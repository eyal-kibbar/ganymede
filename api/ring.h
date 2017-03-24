#ifndef RING_H
#define RING_H

#include <inttypes.h>
#include "ganymede.h"

typedef struct
{
    volatile uint8_t* buff;          /**< ring's buffer */
    uint16_t buff_sz;       /**< ring's buffer size */
    volatile uint16_t head;          /**< index of the ring's next element */
    volatile uint16_t tail;          /**< index if the ring's last element */
}
ring_t;


void ring_init(ring_t* ring, uint8_t* buff, uint16_t buff_sz);

void ring_enqueue(ring_t* ring, uint8_t data);
void ring_dequeue(ring_t* ring, uint8_t* data);

int ring_is_empty(ring_t* ring);
int ring_is_full(ring_t* ring);

int ring_size(ring_t* ring);

#endif /* RING_H */
