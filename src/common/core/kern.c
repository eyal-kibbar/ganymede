#include "kern.h"
#include "ganymede.h"
#include "sched.h"
#include "timer.h"
#include "platform.h"

#include "uart_drv.h"
#include "log.h"
#include "uart.h"
#include <stdio.h>

#include <avr/io.h>


__attribute__((optimize("Os")))
int main()
{
    platform_cli();

    uart_init();
    log_init();
    timer_init();

    sched_init();

    wd_init();
    platform_sei();



    while (1) {
        sched_loop();
    }
    return 0;
}

