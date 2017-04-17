#include "ring.h"
#include "ganymede_platform.h"

void ring_init(ring_t* ring, uint8_t* buff, uint16_t buff_sz)
{
    ring->buff = buff;
    ring->buff_sz = buff_sz;
    ring->head = ring->tail = 0;
}

void ring_enqueue(ring_t* ring, uint8_t data)
{
    volatile uint16_t head;
    volatile uint16_t epoch;

    epoch = ring->head & 0x8000;

    head = ring->head & (~0x8000);
    ring->buff[head++] = data;

    if (head == ring->buff_sz) {
        head = 0;
        epoch ^= 0x8000;
    }

    ring->head = head | epoch;
}

void ring_dequeue(ring_t* ring, uint8_t* data)
{
    volatile uint16_t tail;
    volatile uint16_t epoch;

    epoch = ring->tail & 0x8000;

    tail = ring->tail & (~0x8000);
    *data = ring->buff[tail++];

    if (tail == ring->buff_sz) {
        tail = 0;
        epoch ^= 0x8000;
    }

    ring->tail = tail | epoch;
}

int ring_is_empty(ring_t* ring)
{
    return ring->head == ring->tail;
}

int ring_is_full(ring_t* ring)
{
    return (ring->head ^ ring->tail) == 0x8000;
}

int ring_size(ring_t* ring)
{
    if ((ring->head ^ ring->tail) & 0x8000) {
        return (ring->head & ~0x8000) + ring->buff_sz - (ring->tail & ~0x8000);
    }
    else {
        return ring->head - ring->tail;
    }
}

