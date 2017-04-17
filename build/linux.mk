
# default tools
# -----------------------------------------------------------------------------
CC        ?= gcc
AR        ?= ar
CFLAGS    ?= -Wall -g

# directories
# -----------------------------------------------------------------------------
DIR_SRCS += \
	$(DIR_PLATFORM) \
	$(DIR_PLATFORM)/drv


DIR_INCS += \
	$(DIR_PLATFORM) \
	$(DIR_PLATFORM)/drv



# objects
# -----------------------------------------------------------------------------
OBJS += ganymede_platform.o

ifneq ($(filter DRV_UART_ENABLED, $(CONFIG_$(FLAVOR))), )
	OBJS += 
endif


