#pragma once
#include <cstdio>
#include <string>
#include <map>
#include <cassert>

extern "C" {
#include "lib/aconv.h"

#define INI_STOP_ON_FIRST_ERROR
#include "extern/inih/ini.h"
}

struct Asset {
    std::string name;
    std::string type;
    std::map<std::string, std::string> metadata;
};
