#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>

#define platform_cli() cli()
#define platform_sei() sei()
#define platform_memset memset
#define platform_memcpy memcpy

#define TIMER_MAX_DELAY_TICKS 4000 // when delay ms = 65536, and 1 tick = 16.384 ms, it is impossible to wait more than 4000 ticks

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)


#define __LOG_PRINT(prefix, module, fmt, ...)   \
    do {                                        \
        printf_P(PSTR(prefix module " # "));    \
        printf_P(PSTR(fmt), ##__VA_ARGS__);     \
        printf_P(PSTR("\n\r"));                 \
    } while(0)

#define __LOG_ASSERT(condition, fmt, ...)                   \
    do {                                                    \
        if (!(condition)) {                                 \
            LOG_PRINT("ASSERT:", __FILE__,                  \
                  "line %d: " fmt, __LINE__, ##__VA_ARGS__);\
            while (1);                                      \
        }                                                   \
    } while (0)


void platform_idle(void);

#endif /* PLATFORM_H_ */

