#include <cstdio>
#include "asset/util.hpp"
extern "C" {
    #include "asset_config.h"
}

void write_makefile(FILE *fp)
{
    /* header */
    fprintf(fp, ".PHONY: assets\n");
    fprintf(fp, "HEADER_OUT_DIR ?= $(BUILD_DIR)/transient/gen\n");
    fprintf(fp, "ASSET_OUT_DIR  ?= $(BUILD_DIR)/transient/assets\n");
    fprintf(fp, "TOOLS_BIN      ?= $(ENGINE_DIR)/tools/bin\n");

    /* main dependency */
    fprintf(fp, "\nASSETS       := ");
    for(int i = 0; i < config_count(Package); ++i) {
        const auto *pak = config_get(Package) + i;
        fprintf(fp, "$(BUILD_DIR)/%s.dat ", pak->name);
    }

    /* packages */
    fprintf(fp, "\n# Packages");
    for(int i = 0; i < config_count(Package); ++i) {
        const auto *pak = config_get(Package) + i;
        const char **assets = pak->contents;
        if(!assets) break;

        fprintf(fp, "\n$(BUILD_DIR)/%s.dat: $(TOOLS_BIN)/mkpak ", pak->name);

        const char **p = assets;
        const char *asset_name;
        while((asset_name = *p++)) {
            bool already_found = false;

            #define find_for(_type) \
            { \
                const auto *asset = config_find_in(asset_name, config_get(_type), config_count(_type)); \
                if(asset) { \
                    if(already_found) { \
                        fprintf(stderr, "mkmak: warning, multiple assets named %s\n", asset_name); \
                    } \
                    fprintf(fp, "$(ASSET_OUT_DIR)/%s.dat ", asset->name); \
                    already_found = true; \
                } \
            }

            do_all(find_for)

            #undef find_for
        }

        fprintf(fp, "\n\t@mkdir -p `dirname $@`\n");
        fprintf(fp, "\t@mkdir -p $(ASSET_OUT_DIR)\n");
        fprintf(fp, "\t@mkdir -p $(HEADER_OUT_DIR)\n");
        fprintf(fp, "\t@$(TOOLS_BIN)/mkpak %s $(ASSET_OUT_DIR) $@ > $(HEADER_OUT_DIR)/assets_%s.gen.h\n", pak->name, pak->name);
        fprintf(fp, "\t@echo \"package '%s' -> $@\"\n", pak->name);
    }

    /* assets */
    int asset_count = 0;

    #define gen_for(_type) \
    for(int i = 0; i < config_count(_type); ++i) { \
        auto *asset = config_get(_type) + i; \
        if(!asset) break; \
        fprintf(fp, "\n"); \
        fprintf(fp, "$(ASSET_OUT_DIR)/%s.dat: $(TOOLS_BIN)/mkasset %s\n", asset->name, asset->path); \
        fprintf(fp, "\t@mkdir -p `dirname $@`\n"); \
        fprintf(fp, "\t@$(TOOLS_BIN)/mkasset $(TARGET_PLATFORM) " #_type " %s %d > $@\n", asset->name, asset_count++); \
        fprintf(fp,  "\t@echo \"$^ -> $@\"\n"); \
    }

    fprintf(fp, "\n# Assets");
    do_all(gen_for)

    #undef gen_for
}

int main(int argc, char **argv)
{
    write_makefile(stdout);
}
