#include "log.h"
#include "ganymede_platform.h"

__attribute__((optimize("Os")))
void log_init(void)
{
    log_set_mode(LOG_MODE_SYNC);
}


