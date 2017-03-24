#include "log.h"
#include "uart.h"
#include "platform.h"
#include <stdio.h>
#include "uart_drv.h"

#if LOG_UART_ASYNC == 0

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

static FILE log_uart_stream = FDEV_SETUP_STREAM(
    log_uart_sync_putchar,
    log_uart_sync_getchar,
    _FDEV_SETUP_RW);

#else /* LOG_UART_ASYNC != 0 */

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

static FILE log_uart_stream = FDEV_SETUP_STREAM(
    log_uart_async_putchar,
    log_uart_async_getchar,
    _FDEV_SETUP_RW);

#endif /* LOG_UART_ASYNC */

__attribute__((optimize("Os")))
void log_init(void)
{
    stdout = &log_uart_stream;
    stdin = &log_uart_stream;
}
