SHELL := /bin/bash

.PHONY: prebuild postbuild cleanhook preclean engine game
.DEFAULT_GOAL := all

ifeq ($(ENGINE_DIR),)
$(error ENGINE_DIR not set)
endif
ifeq ($(BUILD_DIR),)
$(error BUILD_DIR not set)
endif

COMMON_INCLUDE     = $(ENGINE_DIR)/src
COMMON_ENGINE_SRC  = engine/core.c engine/event.c engine/render.c platform/filesys.c

BUILD_DIR         := $(BUILD_DIR)/$(TARGET_PLATFORM)
OBJ_DIR 		   = $(BUILD_DIR)/tmp/objs
############################
# Include platform makefile #
ifeq ($(TARGET_PLATFORM), unix)
include $(ENGINE_DIR)/makefiles/unix-platform.mak
else ifeq ($(TARGET_PLATFORM), dos)
include $(ENGINE_DIR)/makefiles/dos-platform.mak
else
$(error Unsupported platform or TARGET_PLATFORM not specified)
endif
# ************************** #
############################

preclean: cleanhook
	@[[ $(BUILD_DIR) == /* ]] || rm -Rf $(BUILD_DIR)
	@find . -type f -name '._*' -delete

mainbuild: prebuild
	@echo "*** Build ***"
	@$(MAKE) --no-print-directory build

all: postbuild
