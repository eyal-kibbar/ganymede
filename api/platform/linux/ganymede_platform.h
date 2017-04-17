#ifndef PLATFORM_H_
#define PLATFORM_H_
#define TIMER_MAX_DELAY_TICKS 4000 // when delay ms = 65536, and 1 tick = 16.384 ms, it is impossible to wait more than 4000 ticks

#include <string.h>

void platform_cli();
void platform_sei();


#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)


#define __LOG_PRINT(prefix, module, fmt, ...)   \
    do {                                        \
        printf(prefix module " # ");    \
        printf(fmt, ##__VA_ARGS__);     \
        printf("\n\r");                 \
    } while(0)


#define __LOG_ASSERT(condition, fmt, ...)                   \
    do {                                                    \
        if (!(condition)) {                                 \
            LOG_PRINT("ASSERT:", __FILE__,                  \
                  "line %d: " fmt, __LINE__, ##__VA_ARGS__);\
            while (1);                                      \
        }                                                   \
    } while (0)

#endif /* PLATFORM_H_ */

