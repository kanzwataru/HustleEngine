.PHONY: all clean
.DEFAULT_GOAL := all

BUILD_DIR	:= build
OBJ_DIR		:= $(BUILD_DIR)/transient/obj
INCLUDE_DIR	:= $(ENGINE_DIR)/src/ $(GAME_INCLUDE) $(BUILD_DIR)/transient/gen
TOOLS_DIR	:= $(ENGINE_DIR)/tools
TOOLS_BIN	:= $(TOOLS_DIR)/bin

DEFINES		+=
ENGINE_SRC	:= core/asset.c \
			   common/mathlib.c \
			   engine/sprite.c

ASSET_CONFIG_FILE := config/asset_config.c
SETTINGS_MK := $(BUILD_DIR)/transient/make/settings.mk
ASSETS_MK	:= $(BUILD_DIR)/transient/make/assets.mk

all: postbuild

clean: cleanhook tools_clean
	@rm -Rf $(BUILD_DIR)
	@rm -f `find . -name "._*"`
	@echo "Cleaned build"

# gives targets 'tools_clean' and each tool
include $(ENGINE_DIR)/tools/tools.mk

$(SETTINGS_MK): $(TOOLS_BIN)/mkbuildconf
	@mkdir -p `dirname $@`
	@$(TOOLS_BIN)/mkbuildconf $@

$(ASSETS_MK): $(SETTINGS_MK) $(TOOLS_BIN)/mkmak
	@mkdir -p `dirname $@`
	@$(ENGINE_DIR)/tools/bin/mkmak >> $@

ifneq ($(MAKECMDGOALS), clean)
# gives target 'assets'
include $(ASSETS_MK)

# also includes relevant platform makefile
# platform provides 'game' 'run' 'debug'
include $(SETTINGS_MK)
endif
