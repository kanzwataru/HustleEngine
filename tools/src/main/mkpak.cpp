#include <cstdio>
#include <cassert>
#include <string>
#include <cstdint>
#include "asset/util.hpp"
extern "C" {
    #include "asset_config.h"
}

void help(void)
{
    fprintf(stderr, "mkpak [name] [asset data dir] [out file] > [out header]\n");
}

size_t content_count(const Package *pak)
{
    const char **p = pak->contents;
    if(!p)
        return 0;

    size_t count = 0;
    const char *asset_name;
    while((asset_name = *p++)) {
        ++count;
    }

    return count;
}

std::string to_upper(const std::string &str)
{
    std::string upper = str;

    for(size_t i = 0; i < str.length(); ++i) {
        upper[i] = std::toupper(str[i]);
    }

    return upper;
}

int main(int argc, char **argv)
{
    if(argc < 4) {
        help();
        return 1;
    }

    const char *name = argv[1];
    const char *asset_dir = argv[2];
    const char *out_file = argv[3];

    const auto *pak = config_find_in(name, config_get(Package), config_count(Package));
    if(!pak) {
        fprintf(stderr, "mkpak: Package '%s' not found\n", name);
        return 1;
    }

    FILE *out = fopen(out_file, "w");
    assert(out);

    /* write out beginning of c header */
    fprintf(stdout, "#ifndef ASSETS_%s_GEN_H\n", to_upper(name).c_str());
    fprintf(stdout, "#define ASSETS_%s_GEN_H\n\n", to_upper(name).c_str());

    /* write out empty pak header */
    size_t top = 0;
    uint32_t pak_size = 0;
    fwrite(&pak_size, sizeof(pak_size), 1, out);

    top += sizeof(pak_size);
    for(size_t i = 0; i < content_count(pak); ++i) {
        /* get asset name */
        std::string asset_name = pak->contents[i];

        /* write out asset, with padding */
        std::string file_name = std::string(asset_dir) + "/" + asset_name + std::string(".dat");
        FILE *asset = fopen(file_name.c_str(), "rb");
        if(!asset) {
            fprintf(stderr, "mkpak: cannot open '%s\n'", file_name.c_str());
            exit(1);
        }

        size_t size = 0;
        int c;
        while(EOF != (c = fgetc(asset))) {
            fputc(c, out);
            ++size;
        }

        size_t padding = 16 - ((top + size) % 16); // pad to 16 bytes
        for(size_t _ = 0; _ < padding; ++_)
            fputc(0, out);

        fclose(asset);

        /* write out offset for header */
        fprintf(stdout, "#define ASSET_%s\t\t%zu\n", to_upper(asset_name).c_str(), top);

        top += size + padding;
    }

    /* end of c header */
    fprintf(stdout, "\n#endif\n");

    /* go back and rewrite the pak header */
    pak_size = top - sizeof(pak_size);
    rewind(out);
    fwrite(&top, sizeof(pak_size), 1, out);

    fclose(out);

    return 0;
}
