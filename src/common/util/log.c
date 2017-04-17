#include "log.h"
#include "ganymede_platform.h"
#include <stdio.h>

extern FILE log_uart_sync_stream;
extern FILE log_uart_async_stream;

__attribute__((optimize("Os")))
void log_init(void)
{
    log_set_mode(LOG_MODE_SYNC);
}

void log_set_mode(log_mode_t mode)
{
    switch (mode) {
    case LOG_MODE_SYNC:
        stdout = &log_uart_sync_stream;
        stdin = &log_uart_sync_stream;
        break;
    case LOG_MODE_ASYNC:
        stdout = &log_uart_async_stream;
        stdin = &log_uart_async_stream;
        break;
    default:
         break;
    }

}
