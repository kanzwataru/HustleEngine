.PHONY: all engine assets tools game clean cleanhook prebuild postbuild run debug
.DEFAULT_GOAL := all

ENGINE_DIR  := $(realpath $(ENGINE_DIR))
BUILD_DIR	:= $(PWD)/build/$(TARGET_PLATFORM)
OBJ_DIR		:= $(BUILD_DIR)/transient/obj
HEADERS		:= $(ENGINE_DIR)/src/ $(GAME_HEADERS)
ENGINE_SRC	:=
DEFINES		+=
SETTINGS     = $(BUILD_DIR)/settings.mk
MKSETTINGS   = $(ENGINE_DIR)/tools/bin/mkbuildconf

all: $(SETTINGS) postbuild

$(SETTINGS): $(MKSETTINGS)
	@mkdir -p `dirname $@`
	@$(ENGINE_DIR)/tools/bin/mkbuildconf $@

$(MKSETTINGS):
	@echo "** Tools **"
	@cd $(ENGINE_DIR)/tools && $(MAKE) --no-print-directory

tools: $(MKSETTINGS)

ifneq ($(MAKECMDGOALS), clean)
include $(SETTINGS)
endif

run: platform_run

clean: cleanhook
	@rm -Rf $(BUILD_DIR)
	@find . -type f -name '._*' -delete
	@echo "Cleaned build"
	@cd $(ENGINE_DIR)/tools && $(MAKE) --no-print-directory clean

debug: platform_debug

game: assets $(SETTINGS)

assets: engine

engine: prebuild tools platform_build
