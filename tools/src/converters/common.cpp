#include "converters/converters.hpp"
#include "engine/asset.h"

std::string common::file_for_asset(std::string name, std::string extension)
{
    std::string filename = "artsrc/" + name + '.' + extension;
    std::FILE *fp = std::fopen(filename.c_str(), "rb");
    if(!fp) {
        std::fprintf(stderr, "* asset file not found: %s\n", filename.c_str());
        exit(1);
    }

    std::fclose(fp);

    return filename;
}
