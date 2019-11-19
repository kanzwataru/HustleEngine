.PHONY: all clean
.DEFAULT_GOAL := all

# variables for independent building
TOOLS_DIR			?= .
TOOLS_BIN			?= $(TOOLS_DIR)/bin
ASSET_CONFIG_FILE   ?= dummy_asset_conf.c

include tools.mk

all: $(TOOLS_BIN)/mkbuildconf $(TOOLS_BIN)/mkpak $(TOOLS_BIN)/mkasset $(TOOLS_BIN)/mkmak
clean: tools_clean
