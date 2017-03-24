#include "uart.h"
#include "uart_drv.h"
#include "uart_platform.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>


__attribute__((optimize("Os")))
void uart_platform_init(void)
{
    uint16_t baudrate = ((F_CPU / (DRV_UART_BAUDRATE * 16UL)) - 1);

    UBRR0H = (uint8_t)(baudrate >> 0x8);
    UBRR0L = (uint8_t)(baudrate & 0xFF);

    UCSR0B = _BV(TXEN0) | _BV(RXEN0);
}

void uart_set_data(uint8_t data)
{
    UDR0 = data;
}

void uart_irq_set_rx(uint8_t enabled)
{
    if (enabled) {
        UCSR0B |= _BV(RXCIE0);
    }
    else {
        UCSR0B &= ~_BV(RXCIE0);
    }
}

void uart_irq_set_tx(uint8_t enabled)
{
    if (enabled) {
        UCSR0B |= _BV(UDRIE0);
    }
    else {
        UCSR0B &= ~_BV(UDRIE0);
    }
}

int uart_tx_is_ready()
{
    return !!(UCSR0A & _BV(UDRE0));
}

void uart_get_data(uint8_t* data)
{
    *data = UDR0;
}


ISR(USART_UDRE_vect)
{
    uart_isr_tx();
}


ISR(USART_RX_vect)
{
    uart_isr_rx();
}
