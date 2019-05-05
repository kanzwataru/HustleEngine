.PHONY: assets assets_clean

ASSETS := balloon cacp_fa clipper cloud clouds vgapal
PAKS   := assets.dat

PAKS   := $(patsubst %, $(BUILD_DIR)/%, $(PAKS))
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
