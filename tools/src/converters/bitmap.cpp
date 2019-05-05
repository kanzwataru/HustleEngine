#include "converters/converters.hpp"
#include "engine/asset.h"

#define RLE_MAX_SIZE 1280000

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
    uint8_t *rle = (uint8_t *)calloc(1, RLE_MAX_SIZE); // this is ugly and unsafe
    ImageInfo info = {0};
    size_t size;

    validate_spritesheet(asset);

    // load
    std::string filename = common::file_for_asset(asset.name, "bmp");
    image_data = load_bmp_image(filename.c_str(), &info);

    assert(image_data);
    if(!(info.width == std::stoi(asset.metadata.at("width")))) {
        fprintf(stderr, "\n\nIn asset: %s\n", asset.name.c_str());
        fprintf(stderr, "Width mismatch: %d <-> %d!!\n\n", info.width, std::stoi(asset.metadata.at("width")));
        exit(1);
    }

    if(!(info.height == std::stoi(asset.metadata.at("height")) * std::stoi(asset.metadata.at("count")))) {
        fprintf(stderr, "\n\nIn asset: %s\n", asset.name.c_str());
        fprintf(stderr, "Height mismatch: %d <-> %d (%d * %d)\n\n",
                info.height,
                std::stoi(asset.metadata.at("height")) * std::stoi(asset.metadata.at("count")),
                std::stoi(asset.metadata.at("height")),
                std::stoi(asset.metadata.at("count")));
        exit(1);
    }

    // convert
    size = buffer_to_rle((RLEImage *)rle, image_data, info.width, info.height);
    assert(size <= RLE_MAX_SIZE); // cringe

    // output
    write_spritesheet(asset, rle, size, out, SHEET_RLE);

    free(rle);
    free(image_data);
}

void palette::bmp2pal(const Asset &asset, std::FILE *out)
{
    uint8_t *pal_data = nullptr;
    size_t size = 256 * 3; // assert mode 13h for now

    std::string filename = common::file_for_asset(asset.name, "bmp");
    pal_data = load_bmp_palette(filename.c_str());
    assert(pal_data);

    fwrite(pal_data, sizeof(uint8_t), size, out);
}
