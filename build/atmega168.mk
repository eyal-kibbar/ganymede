
# default tools
# -----------------------------------------------------------------------------
CC        ?= avr-gcc
AR        ?= avr-ar
CFLAGS    ?= -Wall -DF_CPU=16000000UL -mmcu=atmega168 -g

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
	OBJS += uart_platform.o
endif


