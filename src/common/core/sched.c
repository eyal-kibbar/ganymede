#include "sched.h"
#include "kern.h"
#include "ganymede.h"
#include "ganymede_platform.h"
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

#define CANARY_MAGIC 0xDEAD

#ifdef CANARY
#define CANARY_CHECK() LOG_ASSERT(sched.curr->canary == CANARY_MAGIC, "dead canary: %p", sched.curr)
#else /* CANARY */
#define CANARY_CHECK()
#endif /* CANARY */

typedef enum task_state_e {
    TASK_STATE_RUNNING,
    TASK_STATE_IDLE,
    TASK_STATE_DELAYED,
    TASK_STATE_WAITING
} task_state;


typedef struct task_s {
    task_info_t info;
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

    uint8_t ctx[SCHED_CONTEXT_SIZE];

#ifdef CANARY
    uint16_t canary;
#endif /* CANARY */
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
    task_t* prev = sched.curr;

    CANARY_CHECK();

    sched.curr = next;
    sched.curr->state = TASK_STATE_RUNNING;

    CANARY_CHECK();

    platform_context_swap((sched_context_t)prev->ctx, (sched_context_t)next->ctx);
}

__attribute__((noreturn))
static void task_start(void)
{
    LOG_INFO(SCHED, "first time");

    while (1) {
        sched.curr->info.loop_func();
    }
}

__attribute__((optimize("Os")))
void sched_init(void)
{
    task_t* task;
    uint8_t tid = 0;
    extern char __tasks_end[];
    extern char __tasks_start[];

    sched.first = (task_t*)__tasks_start;

    //LOG_INFO(SCHED, "tasks start: %p end: %p", __tasks_start, __tasks_end);


    for (task = sched.first; task; task = (task_t*)task->info.next) {

        platform_context_create((sched_context_t)task->ctx, task_start, (void*)task, (size_t)task->info.stack_size);

        task->info.next = (void*)((uintptr_t)task + (uintptr_t)task->info.stack_size);

        task->tid = ++tid;
        task->info.setup_func();

        task->state = TASK_STATE_IDLE;

        if (task->info.next == __tasks_end){
            task->info.next = NULL;
        }

#ifdef CANARY
        task->canary = CANARY_MAGIC;
#endif /* CANARY */
    }

#ifdef CANARY
        sched.scheduler.canary = CANARY_MAGIC;
#endif /* CANARY */

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

