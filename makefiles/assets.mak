.PHONY: assets assets_clean

ASSETS := $(shell $(ENGINE_DIR)/bin/lsini config/assets.ini)
PAKS   := $(shell $(ENGINE_DIR)/bin/lsini config/pak.ini)

PAKS   := $(patsubst %, $(BUILD_DIR)/%.dat, $(PAKS))
ASSBIN := $(patsubst %, build/__temp__/%.bin, $(ASSETS))
TOOLS  := ../bin

build/__temp__/%.bin:
	@mkdir -p $(dir $@)
	@$(TOOLS)/mkasset $(patsubst build/__temp__/%.bin, %, $(@)) > $@

$(BUILD_DIR)/%.dat: $(ASSBIN)
	@mkdir -p $(BUILD_DIR)
	@$(TOOLS)/mkpak > $@

assets: $(PAKS)

assets_clean:
	@rm -f $(ASSBIN)
	@rm -f $(PAKS)
