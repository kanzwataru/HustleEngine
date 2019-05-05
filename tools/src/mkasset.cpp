/*
 * HustleEngine Asset Converter
 * MKASSET
*/
#include "mkasset.hpp"
#include "converters/converters.hpp"

typedef void (*handler_t)(const Asset &, std::FILE *);

static const std::map<std::string, handler_t> handlers = {
    {"SpritesheetRLE", spritesheet::bmp2rle},
    {"Palette", palette::bmp2pal},
};

static void help(bool verbose)
{
    const char *verbose_help_line =
    "Converts assets from their respective formats to\n"
    "HustleEngine data, including metadata specified in INI files\n"
    "\n\n"
    " example: \n"
    " [foo]\n"
    " type=SpritesheetRLE\n"
    " width=64\n"
    " height=64\n"
    " count=4\n"
    " frameskip=3\n"
    "\n"
    " [bar]\n"
    " type=adlib\n"
    "\n"
    " mkasset foo > foo.bin\n"
    " mkasset bar > bar.bin\n";

    const char *usage_line =
    "HustleEngine mkasset utility\n\n"
    "Usage: mkasset <name>\n"
    "(pass -h for more info)";

    if(verbose)
        std::fprintf(stderr, verbose_help_line);
    else
        std::fprintf(stderr, usage_line);
}

static int ini_callback(void *user, const char *section, const char *name, const char *value)
{
    Asset *asset = (Asset *)user;

    if(asset->name != section) {
        return 1; /* keep going */
    }

    if(std::string(name) == "type") {
        asset->type = value;
    }
    else {
        asset->metadata[name] = value;
    }

    return 1;
}

static void convert_asset(std::string name)
{
    Asset asset;

    // load asset info from ini file
    asset.name = name;

    if(ini_parse("config/assets.ini", ini_callback, &asset) < 0) {
        std::fprintf(stderr, "\n * Can't correctly load assets.ini\n");
        exit(1);
    }

    // ensure validity
    if(asset.type == "" || !handlers.count(asset.type)) {
        std::fprintf(stderr, "\n * Invalid or missing asset: %s of type %s\n", asset.name.c_str(), asset.type.c_str());
        exit(1);
    }

    // run handler and output to stdout
    handlers.at(asset.type)(asset, stdout);
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        help(false);
        return 2;
    }

    if(std::string(argv[1]) == "-h") {
        help(true);
        return 2;
    }

    convert_asset(argv[1]);
}
