.PHONY: all engine assets tools game clean cleanhook prebuild postbuild run
.DEFAULT_GOAL := all

ENGINE_DIR  := $(realpath $(ENGINE_DIR))
BUILD_DIR	:= $(PWD)/build/$(TARGET_PLATFORM)
OBJ_DIR		:= $(BUILD_DIR)/transient/obj
INCLUDE		:= $(ENGINE_DIR)/src/ $(GAME_INCLUDE)
ENGINE_SRC	:=
DEFINES		+=

#####
# include platform makefile
ifeq ($(TARGET_PLATFORM), unix)
include $(ENGINE_DIR)/makefiles/unix-platform.mak
#else ifeq ($(TARGET_PLATFORM), dos)
#include $(ENGINE_DIR)/makefiles/dos-platform.mak
else
$(error Unsupported platform or TARGET_PLATFORM not specified)
endif
#####

all: postbuild

run: platform_run

clean: cleanhook
	@rm -Rf $(BUILD_DIR)
	@find . -type f -name '._*' -delete

run: all

game: assets

assets: engine

engine: prebuild tools platform_build

tools:
	@cd $(ENGINE_DIR)/tools && make
