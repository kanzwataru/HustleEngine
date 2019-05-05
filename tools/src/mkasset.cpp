/*
 * HustleEngine Asset Converter
 * MKASSET
*/
#include "mkasset.hpp"
#include "converters/converters.hpp"

typedef void (*handler_t)(Asset, std::FILE *);

static const std::map<std::string, handler_t> handlers = {
    {"SpritesheetRLE", spritesheet::bmp2rle}
};

void help()
{
    const char *help_line =
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

    std::fprintf(stderr, help_line);
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        help();
        return 1;
    }

    // load asset info from ini file

    // load asset data

    // convert asset

    // write asset out to stdout
}
