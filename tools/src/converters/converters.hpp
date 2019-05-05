#pragma once
#include "mkasset.hpp"

namespace common {
    std::string file_for_asset(std::string name, std::string extension);
}

namespace spritesheet {
    void bmp2rle(const Asset &asset, std::FILE *out);
}

namespace palette {
    void bmp2pal(const Asset &asset, std::FILE *out);
}
