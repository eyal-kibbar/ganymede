#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

void timer_init(void);

uint16_t timer_get_ticks(void);

uint16_t timer_ms2ticks(uint16_t ms, uint16_t* remaining_us);

#endif /* TIMER_H_ */
