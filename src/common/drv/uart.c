#include "ganymede_platform.h"
#include "uart_drv.h"
#include "uart_platform.h"
#include "ring.h"

typedef struct uart_s {
    ring_t tx_ring;
    uint8_t tx_buff[UART_TX_BUFF_LEN];
    semaphore_t tx_sem;

    ring_t rx_ring;
    uint8_t rx_buff[UART_RX_BUFF_LEN];
    semaphore_t rx_sem;
} uart_t;

static uart_t uart;

__attribute__((optimize("Os")))
void uart_init(void)
{
    uart_platform_init();

    ring_init(&uart.tx_ring, uart.tx_buff, UART_TX_BUFF_LEN);
    semaphore_init(&uart.tx_sem, 1);

    ring_init(&uart.rx_ring, uart.rx_buff, UART_RX_BUFF_LEN);
    semaphore_init(&uart.rx_sem, 0);
}

void uart_sync_send(uint8_t* buff, uint8_t len)
{
    for (;len; ++buff, --len) {

        while (!uart_tx_is_ready());

        uart_set_data(*buff);
    }
}

int uart_sync_recv(uint8_t* buff, uint8_t len)
{
    return 0;
}

void uart_async_send(uint8_t* buff, uint8_t len)
{
    int n;

    for(n = 0; n < len;) {

        // wait for the ring to have an emoty slot
        semaphore_wait(&uart.tx_sem);

        // disable tx interrupt in ortder to avoid race on tx_ring
        uart_irq_set_tx(0);

        // fill ring with the buffer
        for (; !ring_is_full(&uart.tx_ring) && n < len; ++n, ++buff) {
            ring_enqueue(&uart.tx_ring, *buff);
        }

        // signal this ring still has empty slots if it is not full
        if (!ring_is_full(&uart.tx_ring)) {
            semaphore_signal(&uart.tx_sem);
        }

        // enable tx interrupt in order to allow the tx isr to transmit
        uart_irq_set_tx(1);
    }
}

int uart_async_recv(uint8_t* buff, uint8_t len)
{
    int n;

    for(n = 0; n < 1;) {

        // enable rx interrupt in order to allow the rx isr to fill the ring
        uart_irq_set_rx(1);

        // wait for ring to have at least 1 element
        semaphore_wait(&uart.rx_sem);

        // disable rx interrupt in order to avoid race on rx_ring
        uart_irq_set_rx(0);

        // empty the ring into the buffer
        for (; !ring_is_empty(&uart.rx_ring) && n < len; ++n, ++buff) {
            ring_dequeue(&uart.rx_ring, buff);
        }

        // mark that the ring still has more elements if it does
        if (!ring_is_empty(&uart.rx_ring)) {
            semaphore_signal(&uart.rx_sem);
        }
    }

    return n;
}

void uart_isr_rx(void)
{
    uint8_t data;

    // stop receiving if the rx ring is full
    if (ring_is_full(&uart.rx_ring)) {
        uart_irq_set_rx(0);
        return;
    }

    // read received byte
    uart_get_data(&data);

    // signal the rx ring is no linger empty
    if (ring_is_empty(&uart.rx_ring)) {
        semaphore_signal(&uart.rx_sem);
    }

    // enqueue the received byte
    ring_enqueue(&uart.rx_ring, data);
}


void uart_isr_tx(void)
{
    uint8_t data;

    // stop transmitting if there is nothing in the ring
    if (ring_is_empty(&uart.tx_ring)) {
        uart_irq_set_tx(0);
        return;
    }

    // signal the tx ring is no longer full
    if (ring_is_full(&uart.tx_ring)) {
        semaphore_signal(&uart.tx_sem);
    }

    // dequeue a byte and transmit it
    ring_dequeue(&uart.tx_ring, &data);
    uart_set_data(data);
}

