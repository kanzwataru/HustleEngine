#pragma once
#include <cstdio>
#include <string>
#include <map>

extern "C" {
#include "lib/aconv.h"
#include "extern/inih/ini.h"
}

struct Asset {
    std::string name;
    std::string type;
    std::map<std::string, std::string> metadata;
};
