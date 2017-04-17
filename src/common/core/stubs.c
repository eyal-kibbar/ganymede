#include "uart_drv.h"
#include "log.h"

void nop(void)
{
}


void uart_init(void) __attribute__((weak, alias("nop")));
void log_init(void) __attribute__((weak, alias("nop")));
void wd_init(void) __attribute__((weak, alias("nop")));
void platform_init(void) __attribute__((weak, alias("nop")));
void log_set_mode(log_mode_t mode) __attribute__((weak, alias("nop")));


