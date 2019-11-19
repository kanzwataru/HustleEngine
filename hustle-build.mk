.PHONY: all engine assets tools game clean cleanhook prebuild postbuild run debug
.DEFAULT_GOAL := all

ASSET_CONFIG_FILE := config/asset_config.c
ENGINE_DIR  := $(realpath $(ENGINE_DIR))
BUILD_DIR	:= $(PWD)/build/$(TARGET_PLATFORM)
OBJ_DIR		:= $(BUILD_DIR)/transient/obj
HEADERS		:= $(ENGINE_DIR)/src/ $(GAME_HEADERS) $(BUILD_DIR)/transient/gen
ENGINE_SRC	:= core/asset.c \
			   common/mathlib.c
DEFINES		+=
SETTINGS     = $(BUILD_DIR)/transient/make/settings.mk
ASSET_MK	 = $(BUILD_DIR)/transient/make/assets.mk
MKSETTINGS   = $(ENGINE_DIR)/tools/bin/mkbuildconf
ASSET_HEADER = $(BUILD_DIR)/transient/gen/assets.gen.h

all: $(SETTINGS) postbuild

$(ASSET_MK): tools
	@mkdir -p `dirname $@`
	@echo "BUILD_DIR  ?= $(BUILD_DIR)" > $@
	@echo "ENGINE_DIR ?= $(ENGINE_DIR)" >> $@
	@$(ENGINE_DIR)/tools/bin/mkmak >> $@

$(SETTINGS): $(MKSETTINGS)
	@mkdir -p `dirname $@`
	@$(ENGINE_DIR)/tools/bin/mkbuildconf $@

tools:
	@echo "** Tools **"
	@cd $(ENGINE_DIR)/tools && $(MAKE) --no-print-directory ASSET_CONFIG_FILE=$(PWD)/$(ASSET_CONFIG_FILE)

$(MKSETTINGS): tools

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

game: engine $(SETTINGS)

$(ASSET_HEADER): $(ASSET_MK) tools
	@$(MAKE) --no-print-directory -f $(ASSET_MK)
	@touch $(ASSET_HEADER) # for now

engine: prebuild assets platform_build
