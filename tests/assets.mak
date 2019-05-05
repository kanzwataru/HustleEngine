.phony: all clean

ASSETS := balloon cacp_fa clipper cloud clouds vgapal
ASSBIN := $(patsubst %, build/__temp__/%.bin, $(ASSETS))
TOOLS  := ../bin

build/__temp__/%.bin:
	mkdir -p $(dir $@)
	$(TOOLS)/mkasset $(patsubst build/__temp__/%.bin, %, $(@)) > $@

RES/assets.pak: $(ASSBIN)
	$(TOOLS)/mkpak unix .

all: RES/assets.pak

clean:
	rm -f $(ASSBIN)
	rm -f RES/assets.pak
