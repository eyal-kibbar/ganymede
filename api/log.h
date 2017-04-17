#ifndef LOG_H
#define LOG_H


#ifdef LOG_ENABLED

#include "log_platform.h"

#define LOG_PRINT(prefix, module, fmt, ...) \
    __LOG_PRINT(prefix, module, fmt, ##__VA_ARGS__)

#define LOG_ASSERT(condition, fmt, ...) \
    __LOG_ASSERT(condition, fmt, ##__VA_ARGS__)

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


typedef enum log_mode_e {
    LOG_MODE_SYNC,
    LOG_MODE_ASYNC
} log_mode_t;

void log_init(void);

void log_set_mode(log_mode_t mode);

#endif /* LOG_H */
