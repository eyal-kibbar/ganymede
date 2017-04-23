#include "ganymede_platform.h"
#include "timer.h"
#include "sched.h"
#include <ucontext.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "log.h"

struct sched_context_s {
    ucontext_t uctx;
};

static volatile uint16_t timer_ticks = 0x0000;
static sem_t interrupts_sem;


void* timer_inc_ticks(void* unused)
{
	struct timespec ts;

	// 1 tick every 16.384 milliseconds
	ts.tv_sec = 0;
	ts.tv_nsec = 16384000;

	(void)unused;

	LOG_INFO(TEST, "starting ticks");

	while (1) {
		platform_cli();
		timer_ticks++;
		platform_sei();

		nanosleep(&ts, NULL);
	}

}

void platform_init()
{
	sem_init(&interrupts_sem, 0, 1);
}


void platform_cli()
{
	sem_wait(&interrupts_sem);
}


void platform_sei()
{
	sem_post(&interrupts_sem);
}


/******************************************************************************/
/** sched                                                                    **/
/******************************************************************************/
void platform_context_swap(sched_context_t save, sched_context_t load)
{
	swapcontext(&save->uctx, &load->uctx);
}

void platform_context_create(sched_context_t ctx, void (*func)(), void* stack, size_t stack_size)
{
	getcontext(&ctx->uctx);
	ctx->uctx.uc_stack.ss_sp = stack;
	ctx->uctx.uc_stack.ss_size = stack_size;

	makecontext(&ctx->uctx, func, 0);
}


void timer_init(void)
{
	pthread_t tid;

	// start timer ticks
	pthread_create(&tid, NULL, timer_inc_ticks, NULL);
}

uint16_t timer_get_ticks(void)
{
    return timer_ticks;
}

uint16_t timer_ms2ticks(uint16_t ms, uint16_t* remaining_us)
{
    uint32_t us = (uint32_t)ms * 1000UL;
    uint16_t ticks = (uint16_t)(us / 16384UL);
    *remaining_us = us - (ticks * 16384UL);
    return ticks;
}

uint16_t timer_ticks2ms(uint16_t ticks)
{
    uint16_t ms;
    ms = ticks * 16;
    ms += (ticks * 3) / 10;
    ms += (ticks * 8) / 100;
    ms += (ticks * 4) / 1000;
    return ms;
}

void platform_idle()
{
}


void platform_irq_get_enabled(uint8_t* irq_status)
{
	int sval;

	sem_getvalue(&interrupts_sem, &sval);

    *irq_status = (uint8_t)!!sval;
}

