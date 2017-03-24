#include "kern.h"
#include "sched.h"
#include "timer.h"
#include "platform.h"

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

typedef struct
{
    uint8_t  regs[16];
    uint16_t bp;
    uint16_t sp;
    uint16_t sreg;
    uint16_t pc;
} __attribute__((packed))
avr_jmp_buf_t;

static volatile uint16_t timer_ticks = 0xFF00;

void platform_set_sp(jmp_buf jmpb, void* sp)
{
    ((avr_jmp_buf_t*)jmpb)->sp = (uint16_t)sp;
}

void platform_irq_get_enabled(uint8_t* irq_status)
{
    *irq_status = !!(SREG & _BV(7));
}

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


uint16_t timer_get_ticks(void)
{
    return timer_ticks;
}

ISR(TIMER2_OVF_vect)
{
    ++timer_ticks;
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


void __attribute__((naked)) __attribute__ ((section (".init0"))) wd_setup(void)
{
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


