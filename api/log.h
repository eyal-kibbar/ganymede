#ifndef LOG_H
#define LOG_H


#ifdef LOG_ENABLED

#include "platform.h"

#define LOG_PRINT(prefix, module, fmt, ...) \
    __LOG_PRINT(prefix, module, fmt, ##__VA_ARGS__)

#define LOG_ASSERT(prefix, fmt, ...) \
    __LOG_ASSERT(prefix, fmt, ##__VA_ARGS__)

#else /* ! LOG_ENABLED */

#define LOG_PRINT(prefix, module, fmt, ...)
#define LOG_ASSERT(condition, fmt, ...)

#endif /* LOG_ENABLED */



#define LOG_INFO(module, fmt, ...) \
    LOG_PRINT("INFO: ", #module, fmt, ##__VA_ARGS__)

#define LOG_ERROR(module, fmt, ...) \
    LOG_PRINT("ERROR: ", #module, fmt, ##__VA_ARGS__)

#define LOG_WARN(module, fmt, ...) \
    LOG_PRINT("WARNING: ", #module, fmt, ##__VA_ARGS__)


void log_init(void);


#endif /* LOG_H */
