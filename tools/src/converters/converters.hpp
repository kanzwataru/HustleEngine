#pragma once
#include "mkasset.hpp"

/* Spritesheet converters */
namespace spritesheet {
    void bmp2rle(Asset asset, std::FILE *out);
}
