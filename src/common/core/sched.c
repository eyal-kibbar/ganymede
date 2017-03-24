#include "sched.h"
#include "kern.h"
#include "ganymede.h"
#include "platform.h"
#include "timer.h"

#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>

#include "log.h"

//#define DEBUG_SCHED

#ifdef DEBUG_SCHED
#define LOG_SCHED(fmt, ...) LOG_INFO(SCHED, fmt, ##__VA_ARGS__)
#else /* ! DEBUG_SCHED */
#define LOG_SCHED(fmt, ...)
#endif


typedef enum task_state_e {
    TASK_STATE_RUNNING,
    TASK_STATE_IDLE,
    TASK_STATE_DELAYED,
    TASK_STATE_WAITING
} task_state;


typedef struct task_s {
    task_info_t info;
    jmp_buf* p_env;
    uint8_t tid;
    task_state state;
    union
    {
        struct
        {
            void* sem;
        }
        waiting;

        struct
        {
            uint16_t end_tick;
        }
        delayed;
    };
}
task_t;

typedef struct sched_s {
    task_t scheduler;
    task_t* first;
    task_t* curr;
} sched_t;


static sched_t sched;

static void sched_task_switch(task_t* next)
{
    jmp_buf env;
    sched.curr->p_env = &env;

    if (0 == setjmp(*(sched.curr->p_env))) {
        sched.curr = next;
        sched.curr->state = TASK_STATE_RUNNING;
        longjmp(*next->p_env, 1);
    }
}

__attribute__((noreturn))
static void task_start(void)
{
    uint16_t next_tick;
    uint16_t us;
    uint16_t carry_us;
    uint16_t now;

    next_tick = timer_get_ticks();
    carry_us = 0;

    while (1) {
        sched.curr->info.loop_func();

        if (sched.curr->info.cycle_ms == 0) {
            next_tick = timer_get_ticks();
            carry_us = 0;
            continue;
        }

        next_tick += timer_ms2ticks(sched.curr->info.cycle_ms, &us);
        carry_us += us;
        next_tick += timer_ms2ticks(carry_us / 1000, &us);
        carry_us = (carry_us % 1000) + us;

        // check if we are falling behind
        now = timer_get_ticks();

        LOG_SCHED("carry us %u %04x %04x\n\r", carry_us, now, next_tick);

        if ((next_tick ^ now) & 0x8000) {
            uint16_t epoch = now & 0x8000;

            now &= ~0x8000;
            next_tick &= ~0x8000;

            if (next_tick > now && (0x7FFF - next_tick + now) < TIMER_MAX_DELAY_TICKS) {
                next_tick = now | epoch;
                carry_us = 0;
                LOG_SCHED("falling behind epoch: %04x\n\r", next_tick);
            }

        }
        else if (next_tick < now) {
            next_tick = now;
            carry_us = 0;
            LOG_SCHED("falling behind: %04x\n\r", next_tick);
        }

        sched.curr->delayed.end_tick = next_tick;
        platform_cli();
        sched.curr->state = TASK_STATE_DELAYED;
        sched_task_switch(&sched.scheduler);
        platform_sei();
    }
}

__attribute__((optimize("Os")))
void sched_init(void)
{
    task_t* task;
    uint8_t* sp;
    uint8_t tid = 0;
    extern char __tasks_end[];
    extern char __tasks_start[];

    sched.first = (task_t*)__tasks_start;

    //LOG_INFO(SCHED, "tasks start: %p end: %p", __tasks_start, __tasks_end);


    for (task = sched.first; task; task = (task_t*)task->info.next) {

        task->info.next = (void*)((uintptr_t)task + (uintptr_t)task->info.stack_size);

        //LOG_INFO(SCHED, "task %p\n\r", task);
        task->tid = ++tid;
        task->info.setup_func();

        task->state = TASK_STATE_IDLE;

        sp = (uint8_t*)((uintptr_t)task->info.next - sizeof(uintptr_t));
        task->p_env = (jmp_buf*)(sp - sizeof(jmp_buf));

        if (0 != setjmp(*task->p_env)) {
            task_start();
            // Should never return
        }

        platform_set_sp(*task->p_env, sp);

        if (task->info.next == __tasks_end){
            task->info.next = NULL;
        }
    }

    sched.curr = &sched.scheduler;
    sched.curr->state = TASK_STATE_RUNNING;
}

void sched_delay(uint16_t ms)
{
    uint16_t us;

    if (ms != 0) {
        sched.curr->delayed.end_tick = timer_ms2ticks(ms, &us) + timer_get_ticks();
        sched.curr->delayed.end_tick += !!us;
        platform_cli();
        sched.curr->state = TASK_STATE_DELAYED;
    }
    else {
        platform_cli();
        sched.curr->state = TASK_STATE_IDLE;
    }

    sched_task_switch(&sched.scheduler);
    platform_sei();
}

void sched_loop(void)
{
    task_t* task;
    uint16_t tick, end_tick;
    uint8_t maysleep = 1;

    // run all idle tasks
    for (task = sched.first; task; task = (task_t*)task->info.next) {

        platform_cli();
        if (TASK_STATE_IDLE == task->state) {
            sched.curr->state = TASK_STATE_IDLE;
            sched_task_switch(task);
        }

        platform_sei();

        wd_reset();
    }

    for (maysleep = 1; maysleep; platform_idle()) {

        // resume delayed tasks
        for (task = sched.first; task; task = (task_t*)task->info.next) {

            if (TASK_STATE_DELAYED == task->state) {

                tick = timer_get_ticks();
                end_tick = task->delayed.end_tick;

                // check if we are in the same epoch
                if (((end_tick ^ tick) & 0x8000)) {

                    end_tick &= ~0x8000;
                    tick &= ~0x8000;

                    // check if end_tick is in the future
                    if (end_tick < tick && (0x7FFF - tick + end_tick) <= TIMER_MAX_DELAY_TICKS) {
                        continue;
                    }
                }
                else if (task->delayed.end_tick > tick) {
                    continue;
                }

                task->state = TASK_STATE_IDLE;
                maysleep = 0;
            }
            else if (TASK_STATE_IDLE == task->state) {
                maysleep = 0;
            }
        }

        wd_reset();
    }
}

void semaphore_init(semaphore_t* sem, uint8_t counter)
{
    sem->counter = counter;
}

void semaphore_wait(semaphore_t* sem)
{
    platform_cli();

    while (sem->counter == 0) {
        sched.curr->state = TASK_STATE_WAITING;
        sched.curr->waiting.sem = sem;
        sched_task_switch(&sched.scheduler);
    }

    --sem->counter;
    platform_sei();
}

void semaphore_signal(semaphore_t* sem)
{
    task_t* task;
    uint8_t irq_status;

    // save global interrupt state
    platform_irq_get_enabled(&irq_status);

    platform_cli();
    ++sem->counter;

    for (task = sched.first; task; task = (task_t*)task->info.next) {
        if (task->state == TASK_STATE_WAITING && task->waiting.sem == sem) {
            task->state = TASK_STATE_IDLE;
        }
    }

    // restore interrupt state
    if (irq_status) {
        platform_sei();
    }
}

uint8_t sched_self(void)
{
    return sched.curr->tid;
}

void sched_set_cycle(uint8_t tid, uint16_t ms)
{
    task_t* task;
    for (task = sched.first; task; task = (task_t*)task->info.next) {
        if (task->tid != tid) {
            continue;
        }

        task->info.cycle_ms = ms;
        return;
    }
}

