#include "drv/uart.h"
#include "uart_drv.h"
#include "uart_platform.h"

#include <stdio.h>


__attribute__((optimize("Os")))
void uart_platform_init(void)
{
}

void uart_set_data(uint8_t data)
{

}

void uart_irq_set_rx(uint8_t enabled)
{

}

void uart_irq_set_tx(uint8_t enabled)
{

}

int uart_tx_is_ready()
{
    return 1;
}

void uart_get_data(uint8_t* data)
{

}

/*
ISR(USART_UDRE_vect)
{
    uart_isr_tx();
}


ISR(USART_RX_vect)
{
    uart_isr_rx();
}
*/
