#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void uart_sync_send(uint8_t* buff, uint8_t len);
int uart_sync_recv(uint8_t* buff, uint8_t len);

void uart_async_send(uint8_t* buff, uint8_t len);
int uart_async_recv(uint8_t* buff, uint8_t len);

#endif /* UART_H_ */

