.phony: all

ASSETS := balloon cacp_fa clipper cloud clouds vgapal
BIN    := $(patsubst %, build/__temp__/%.bin, $(ASSETS))
TOOLS  := ../bin

build/__temp__/%.bin:
	mkdir -p $(dir $@)
	$(TOOLS)/mkasset $(patsubst build/__temp__/%.bin, %, $(@)) > $@

RES/assets.pak: $(BIN)
	$(TOOLS)/mkpak unix .

all: RES/assets.pak
