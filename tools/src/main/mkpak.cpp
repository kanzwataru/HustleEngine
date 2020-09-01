#include <cstdio>
#include <cassert>
#include <string>
#include <cstdint>
#include "asset/util.hpp"
extern "C" {
    #include "asset_config.h"
    #include "asset/asset_types.h"
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

std::string to_upper_first(const std::string &str)
{
    std::string upper = str;

    if(str.length() >= 1) {
        upper[0] = std::toupper(upper[0]);
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

    FILE *out = fopen(out_file, "wb");
    assert(out);

    /* write out beginning of c header */
    fprintf(stdout, "#ifndef ASSETS_%s_GEN_H\n", to_upper(name).c_str());
    fprintf(stdout, "#define ASSETS_%s_GEN_H\n\n", to_upper(name).c_str());
    fprintf(stdout, "#include \"core/asset_structs.h\"\n");
    fprintf(stdout, "struct Pak%s {\n", to_upper_first(name).c_str());

    /* write out empty pak header */
    uint32_t pak_size = 0;
    fwrite(&pak_size, sizeof(pak_size), 1, out);

    size_t top = 0;
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

        /*
        size_t size = 0;
        int c;
        while(EOF != (c = fgetc(asset))) {
            fputc(c, out);
            ++size;
        }
        */
        fseek(asset, 0, SEEK_END);
        size_t size = ftell(asset);
        rewind(asset);

        assert(size > sizeof(uint16_t));
        uint16_t type;
        fread(&type, sizeof(type), 1, asset);
        assert(type >= 0 && type < ASSET_Total);
        size -= sizeof(uint16_t);

        uint8_t *data = new uint8_t[size];
        size_t bytes_in = fread(data, 1, size, asset);
        assert(bytes_in == size);
        size_t bytes_out = fwrite(data, 1, size, out);
        assert(bytes_out == size);
        delete[] data;

        size_t padding = 16 - ((top + size) % 16); // pad to 16 bytes
        for(size_t _ = 0; _ < padding; ++_)
            fputc(0, out);

        fclose(asset);

        //* write out C header for member */
        size_t data_member_size = (size + padding) - asset_size_table[type];
        assert(data_member_size > 0);
        fprintf(stdout, "\tstruct %sAsset %s;\n", asset_name_table[type], asset_name.c_str());
        fprintf(stdout, "\tbyte %s_data[%zu];\n", asset_name.c_str(), data_member_size);

        top += size + padding;
    }

    /* end of c header */
    fprintf(stdout, "};\n");
    fprintf(stdout, "\n#endif\n");

    /* go back and rewrite the pak header */
    pak_size = top - sizeof(pak_size);
    rewind(out);
    fwrite(&pak_size, sizeof(pak_size), 1, out);

    fclose(out);

    return 0;
}
