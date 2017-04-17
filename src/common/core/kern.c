#include "kern.h"
#include "ganymede.h"
#include "sched.h"
#include "timer.h"
#include "ganymede_platform.h"

#include "uart_drv.h"
#include "log.h"
#include "drv/uart.h"
#include <stdio.h>


__attribute__((optimize("Os")))
int main()
{
    platform_cli();

    uart_init();
    log_init();
    timer_init();

    LOG_INFO(KERN, "ganymede initialized !");

    sched_init();

    wd_init();

    log_set_mode(LOG_MODE_ASYNC);
    platform_sei();

    while (1) {
        sched_loop();
    }
    return 0;
}

