#ifndef UART_DRV_H_
#define UART_DRV_H_

#include <stdint.h>


void uart_init(void);
void uart_platform_init(void);

void uart_set_data(uint8_t data);
void uart_get_data(uint8_t* data);

void uart_isr_rx(void);
void uart_isr_tx(void);

void uart_irq_set_rx(uint8_t enabled);
void uart_irq_set_tx(uint8_t enabled);

int uart_tx_is_ready();

#endif /* UART_DRV_H_ */

