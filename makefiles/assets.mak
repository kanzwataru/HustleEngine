.PHONY: assets assets_clean

ASSETS := balloon cacp_fa clipper cloud clouds vgapal
ASSBIN := $(patsubst %, build/__temp__/%.bin, $(ASSETS))
TOOLS  := ../bin

build/__temp__/%.bin:
	@mkdir -p $(dir $@)
	@$(TOOLS)/mkasset $(patsubst build/__temp__/%.bin, %, $(@)) > $@

$(BUILD_DIR)/assets.dat: $(ASSBIN)
	@mkdir -p $(BUILD_DIR)
	@$(TOOLS)/mkpak > $@

assets: $(BUILD_DIR)/assets.dat

assets_clean:
	@rm -f $(ASSBIN)
	@rm -f RES/assets.dat
