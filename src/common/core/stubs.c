#include "uart_drv.h"


void nop_init(void)
{
}


void uart_init(void) __attribute__((weak, alias("nop_init")));
void log_init(void) __attribute__((weak, alias("nop_init")));
void wd_init(void) __attribute__((weak, alias("nop_init")));


