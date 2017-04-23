#ifndef LOG_PLATFORM_H_
#define LOG_PLATFORM_H_

#include <stdio.h>


#define __LOG_PRINT(prefix, module, fmt, ...)   \
    do {                                        \
        printf(prefix module " # ");            \
        printf(fmt, ##__VA_ARGS__);             \
        printf("\n\r");                         \
    } while(0)


#define __LOG_ASSERT(condition, fmt, ...)                   \
    do {                                                    \
        if (!(condition)) {                                 \
            LOG_PRINT("ASSERT:", __FILE__,                  \
                  "line %d: " fmt, __LINE__, ##__VA_ARGS__);\
            while (1);                                      \
        }                                                   \
    } while (0)

#endif /* LOG_PLATFORM_H_ */

