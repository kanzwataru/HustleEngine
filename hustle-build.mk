.PHONY: all engine assets tools game clean cleanhook prebuild postbuild run debug
.DEFAULT_GOAL := all

ENGINE_DIR  := $(realpath $(ENGINE_DIR))
BUILD_DIR	:= $(PWD)/build/$(TARGET_PLATFORM)
OBJ_DIR		:= $(BUILD_DIR)/transient/obj
INCLUDE		:= $(ENGINE_DIR)/src/ $(GAME_INCLUDE)
ENGINE_SRC	:=
DEFINES		+=
SETTINGS     = $(BUILD_DIR)/settings.mk
MKSETTINGS   = $(ENGINE_DIR)/tools/bin/mkbuildconf

all: $(SETTINGS) postbuild

$(SETTINGS): $(MKSETTINGS)
	@mkdir -p `dirname $@`
	@$(ENGINE_DIR)/tools/bin/mkbuildconf $@

$(MKSETTINGS):
	@cd $(ENGINE_DIR)/tools && $(MAKE)

tools:
	@cd $(ENGINE_DIR)/tools && $(MAKE)

ifneq ($(MAKECMDGOALS), clean)
include $(SETTINGS)
endif

run: platform_run

clean: cleanhook
	@rm -Rf $(BUILD_DIR)
	@find . -type f -name '._*' -delete
	@cd $(ENGINE_DIR)/tools && $(MAKE) clean

debug: platform_debug

game: assets $(SETTINGS)
	@echo "Built game"

assets: engine

engine: prebuild tools platform_build
	@echo "Built engine"
