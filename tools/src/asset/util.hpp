#pragma once
#include <cstring>
extern "C" {
    #include "asset_config.h"
}

template <typename T>
const T *config_find_in(const char *name, const T *config_type, size_t count) {
    if(count == 0) {
        return nullptr;
    }

    for(size_t i = 0; i < count; ++i) {
        if(0 == strcmp(config_type[i].name, name))
            return &config_type[i];
    }

    return nullptr;
}
