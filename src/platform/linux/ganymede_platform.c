#include "ganymede_platform.h"
#include "timer.h"
#include <setjmp.h>

void platform_init()
{

    // start timer ticks

    // create interrupt thread and event queue
}


void platform_cli()
{
}


void platform_sei()
{
}


void timer_init(void)
{
}

uint16_t timer_get_ticks(void)
{
    return 0;
}

uint16_t timer_ms2ticks(uint16_t ms, uint16_t* remaining_us)
{
    return 0;
}

void platform_idle()
{
}


void platform_set_sp(jmp_buf jmpb, void* sp)
{
    jmpb[6] = (uintptr_t)sp;
}

void platform_irq_get_enabled(uint8_t* irq_status)
{
    *irq_status = 0;
}
