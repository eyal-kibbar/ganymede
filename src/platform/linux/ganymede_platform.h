#ifndef GANYMEDE_PLATFORM_H_
#define GANYMEDE_PLATFORM_H_

#include <ucontext.h>

#define TIMER_MAX_DELAY_TICKS 4000 // when delay ms = 65536, and 1 tick = 16.384 ms, it is impossible to wait more than 4000 ticks


void platform_cli();
void platform_sei();


#define SCHED_CONTEXT_SIZE (sizeof(ucontext_t))

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)


#endif /* GANYMEDE_PLATFORM_H_ */

