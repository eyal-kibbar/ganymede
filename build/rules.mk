

MODULE_NAME := ganymede
FLAVOR      := $(basename $(notdir $(lastword $(MAKEFILE_LIST))))

# directories
# -----------------------------------------------------------------------------
DIR_MODULE   := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
DIR_BUILD    := $(DIR_MODULE)/build
DIR_PLATFORM := $(DIR_MODULE)/src/platform/$(PLATFORM)
DIR_MODULE_OUT := $(DIR_MODULE)/out
DIR_OUT      := $(DIR_MODULE_OUT)/$(PLATFORM)/$(FLAVOR)
DIR_SRCS     := \
	$(DIR_MODULE)/src/common/core \
	$(DIR_MODULE)/src/common/util \
	$(DIR_MODULE)/src/common/drv

DIR_INCS := \
	$(DIR_MODULE)/inc \
	$(DIR_MODULE)/api \
	$(DIR_MODULE)/api/platform/$(PLATFORM)

DIR_EXTERNAL_INCS :=



# flavors
# -----------------------------------------------------------------------------
ifeq ($(FLAVOR), debug)
	CFLAGS_$(MODULE_NAME)_debug := -O0
	CONFIG_debug := \
		LOG_ENABLED \
		LOG_ASYNC \
		DRV_UART_ENABLED \
		DRV_UART_BAUDRATE=9600 \
		UART_TX_BUFF_LEN=16 \
		UART_RX_BUFF_LEN=16 \
		CANARY

else ifeq ($(FLAVOR), release)
	CFLAGS_$(MODULE_NAME)_release := -Ofast
	CONFIG_release := \
		DRV_UART_ENABLED \
		DRV_UART_BAUDRATE=9600 \
		UART_TX_BUFF_LEN=16 \
		UART_RX_BUFF_LEN=16

else
$(error FLAVOR "$(FLAVOR)" not supported)
endif



# configuration
# -----------------------------------------------------------------------------
CONFIG_$(FLAVOR) += GANYMEDE



# objects
# -----------------------------------------------------------------------------
OBJS := \
	kern.o \
	sched.o \
	ring.o

ifneq ($(filter LOG_ENABLED, $(CONFIG_$(FLAVOR))), )
	OBJS += log.o
endif

ifneq ($(filter DRV_UART_ENABLED, $(CONFIG_$(FLAVOR))), )
	OBJS += uart.o
endif


# platform
# -----------------------------------------------------------------------------
include $(DIR_BUILD)/$(PLATFORM).mk

# must be last
OBJS += stubs.o



# flags
# -----------------------------------------------------------------------------
CFLAGS_$(MODULE_NAME)_$(FLAVOR) += \
	$(addprefix -I, $(DIR_INCS)) \
	$(addprefix -I, $(DIR_EXTERNAL_INCS)) \
	$(addprefix -D, $(CONFIG_$(FLAVOR)))



# rules
# -----------------------------------------------------------------------------

vpath %.c $(DIR_SRCS)


$(DIR_OUT)/%.o: %.c
	$(eval FLAVOR:=$(lastword $(subst /, , $(dir $@))))
	$(CC) -c $(CFLAGS) $(CFLAGS_ganymede) $(CFLAGS_ganymede_$(FLAVOR)) -o $@ $<
	@echo

$(DIR_OUT):
	@mkdir -p $@

$(DIR_MODULE_OUT)/tasks-$(PLATFORM)-$(FLAVOR).o: $(addprefix $(DIR_OUT)/, $(TASKS))
	$(CC) -nostdlib -r -o $@ $^
	@echo

$(DIR_MODULE_OUT)/lib$(MODULE_NAME)-$(PLATFORM)-$(FLAVOR).a: $(addprefix $(DIR_OUT)/, $(OBJS))
	$(AR) rcs $@ $^
	@echo

$(MODULE_NAME)_$(FLAVOR): $(DIR_OUT) $(DIR_MODULE_OUT)/lib$(MODULE_NAME)-$(PLATFORM)-$(FLAVOR).a
	@echo "Done making module $@"
	@echo "=================================================="
	@echo


$(MODULE_NAME)_$(FLAVOR)_clean: $(DIR_OUT)
	$(eval MODULE_NAME :=$(word 1, $(subst _, , $@)))
	$(eval FLAVOR      :=$(word 2, $(subst _, , $@)))
	rm -rf $<
	rm -f  $(abspath $</../../)/lib$(MODULE_NAME)-$(PLATFORM)-$(FLAVOR).a


MODULES += $(MODULE_NAME)_$(FLAVOR)

