

PLATFORM  ?= atmega168

MODULES   :=
DIR_ROOT  := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

all: modules

include $(DIR_ROOT)/build/debug.mk
include $(DIR_ROOT)/build/release.mk

modules: $(MODULES)
	@echo "Done building $^"

clean: $(addsuffix _clean, $(MODULES))
	@echo "Done cleaning"

