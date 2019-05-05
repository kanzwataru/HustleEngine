#include "converters/converters.hpp"
#include "engine/asset.h"

#define MAX_RLE_SIZE 1280000

static void validate_spritesheet(const Asset &asset)
{
    std::string required[] = {
        "width",
        "height",
        "count",
        "frameskip"
    };

    for(const auto &str : required) {
        if(!asset.metadata.count(str)) {
            std::fprintf(stderr, "* asset %s (%s) is missing '%s'!\n", asset.name.c_str(), asset.type.c_str(), str.c_str());
        }
    }
}

static void write_spritesheet(const Asset &asset, uint8_t *data, size_t size, std::FILE *out, byte flags)
{
    Spritesheet sheet_asset;

    sheet_asset.width = std::stoi(asset.metadata.at("width"));
    sheet_asset.height = std::stoi(asset.metadata.at("height"));
    sheet_asset.count = std::stoi(asset.metadata.at("count"));
    sheet_asset.frameskip = std::stoi(asset.metadata.at("frameskip"));
    sheet_asset.flags = flags;

    fwrite(&sheet_asset, sizeof(Spritesheet) - 1, 1, out);
    fwrite(data, sizeof(uint8_t), size, out);
}

void spritesheet::bmp2rle(const Asset &asset, std::FILE *out)
{
    uint8_t *image_data = nullptr;
    uint8_t *rle = (uint8_t *)calloc(1, MAX_RLE_SIZE); // this is ugly and unsafe
    ImageInfo info = {0};
    size_t size;

    validate_spritesheet(asset);

    // get filename
    std::string filename = "artsrc/" + asset.name + ".bmp";
    std::FILE *fp = std::fopen(filename.c_str(), "rb");
    if(!fp) {
        std::fprintf(stderr, "* asset file not found: %s\n", filename.c_str());
    }

    std::fclose(fp); // load_bmp_image has its own file opening

    // load
    image_data = load_bmp_image(filename.c_str(), &info);
    assert(image_data);
    assert(image_data.width == std::stoi(asset.metadata.at("width")));
    assert(image_data.height == std::stoi(asset.metadata.at("height")) * std::stoi(asset.metadata.at("count")));

    // convert
    size = buffer_to_rle((RLEImage *)rle, image_data, info.width, info.height);
    assert(size <= MAX_SIZE); // cringe

    // output
    write_spritesheet(asset, rle, size, out, SHEET_RLE);

    free(rle);
}
