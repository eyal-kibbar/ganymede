#include "kern.h"
#include "sched.h"
#include "timer.h"
#include "ganymede_platform.h"

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <stdio.h>

#include "log.h"

#include <setjmp.h>

#include "drv/uart.h"
#include "uart_drv.h"

struct sched_context_s {
    uint8_t  regs[16];
    uint16_t bp;
    uint16_t sp;
    uint8_t sreg;
    uint16_t pc;
} __attribute__((packed));

static volatile uint16_t timer_ticks = 0xFF00;

/******************************************************************************/
/** sched                                                                    **/
/******************************************************************************/
void platform_context_swap(sched_context_t save, sched_context_t load)
{
    if (0 == setjmp(*(jmp_buf*)save)) {
        longjmp(*(jmp_buf*)load, 1);
    }
}

void platform_context_create(sched_context_t ctx, void (*func)(), void* stack, size_t stack_size)
{
    setjmp(*(jmp_buf*)ctx);

    ctx->pc = (uint16_t)func;
    ctx->sp = (uint16_t)((uintptr_t)stack + stack_size - sizeof(uintptr_t));
    ctx->bp = 0;

    LOG_INFO(SCHED, "pc: %x sp: %x stack size: %d", ctx->pc, ctx->sp, stack_size);
}

void platform_irq_get_enabled(uint8_t* irq_status)
{
    *irq_status = !!(SREG & _BV(7));
}

void platform_idle(void)
{
    cli();
    set_sleep_mode(SLEEP_MODE_IDLE);

    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
}

/******************************************************************************/
/** log                                                                    **/
/******************************************************************************/
static int log_uart_sync_putchar(char c, FILE *stream)
{
    uart_sync_send((uint8_t*)&c, 1);
    return 1;
}

static int log_uart_sync_getchar(FILE *stream)
{
    uint8_t c;
    uart_sync_recv(&c, 1);
    return (int)c;
}

static int log_uart_async_putchar(char c, FILE *stream)
{
    uart_async_send((uint8_t*)&c, 1);
    return 1;
}

static int log_uart_async_getchar(FILE *stream)
{
    uint8_t c;
    uart_async_recv(&c, 1);
    return (int)c;
}

FILE log_uart_sync_stream = FDEV_SETUP_STREAM(
    log_uart_sync_putchar,
    log_uart_sync_getchar,
    _FDEV_SETUP_RW);

FILE log_uart_async_stream = FDEV_SETUP_STREAM(
    log_uart_async_putchar,
    log_uart_async_getchar,
    _FDEV_SETUP_RW);


/******************************************************************************/
/** timer                                                                    **/
/******************************************************************************/
void timer_init(void)
{
    ASSR = _BV(AS2); // set clock to function as realtime hardware clock (RTC)
    TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20); // set clock with 1024 prescalar
    TIMSK2 = _BV(TOIE2); // enable overflow interrupt
    // clock src is 16MHz,
    // we prescale it with 1024, that gives us a counter decrement rate of ~16KHz
    // for every 256 counter decrements we get a tick
    // that gives us tick rate of ~61Hz, or 1 tick every 16.384 ms
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

uint16_t timer_get_ticks(void)
{
    return timer_ticks;
}

ISR(TIMER2_OVF_vect)
{
    ++timer_ticks;
}


/******************************************************************************/
/** watchdog                                                                 **/
/******************************************************************************/
void __attribute__((naked)) __attribute__ ((section (".init0"))) wd_setup(void)
{
    // currently not working due to crapy boot loader: it takes too long to boot
    // and the watch dog bites before it finishes

    //MCUSR = 0;
    //wdt_disable();
}

void wd_reset(void)
{
     wdt_reset();
}

void wd_init(void)
{

    //wdt_reset();

    /* Clear WDRF in MCUSR */
    //MCUSR &= ~_BV(WDRF);

    /* Enable watchdog reset after 8 seconds */
    //wdt_enable(WDTO_8S);

}

