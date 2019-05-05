/*
    ** Example .ini file **

    [balloon]               ; balloon.bmp -> balloon.bin
    type=Spritesheet
    width=32
    height=64
    frames=14
    framerate=12

    ...

    [PakA]
    type=Pak
    Balloon:
    Cloud:
    Foo:
    Bar:



    ** Example generated header **
    #define BALLOON_SPRITE_ASSET    256
    #define CLOUD_SPRITE_ASSET      436
    #define SPRITE_ASSET_COUNT      4


    ** Example file structure **
    config/assets.ini
    config/pak.ini

    -> build/tmp/player_sprite.bin
    -> build/tmp/big_font.bin
    -> build/tmp/intro_song.bin
    ...
    |
    |
    -> build/[platform]/res/paka.dat
       build/[platform]/res/pakb.dat
    -> build/tmp/assets.gen.h

    ** Example pipeline **
    sprite.aseprite -> sprite.bmp -> sprite.rle -> pakb.dat
                                     audio.dat  |
                       font.ttf   -> font.rle   |



    ** output pak file format **
    uint32 size
    byte   data[size]
*/
#include "common/platform.h"
#include "file.h"
#include "extern/inih/ini.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h> /* POSIX only */

#define NAME_SIZE       512
#define MAX_ASSETS      512
#define HEADER_BUF_SIZE 100000
#define PAK_SIZE        64000
#define FILE_EXTENSION  ".pak"

struct Asset {
    char   name[NAME_SIZE];
    char   type[NAME_SIZE];
    size_t size;
    byte   *data;
};

struct Pak {
    char    name[NAME_SIZE];
    size_t  count;
    size_t  size;
    size_t *offsets;
    byte   *data;
    struct Asset **assets;
};

/* globals */
char         *platform;

struct Asset *assets;
size_t        assets_count;
size_t        assets_capacity;

char          header_gen[HEADER_BUF_SIZE];
/* */

int asset_handler(void *user, const char *section, const char *name, const char *value)
{
    struct Asset *asset = assets + (assets_count - 1);

    /* if this is a different asset to the previous one */
    if(strcmp(section, (char *)user) != 0) {
        ++asset;
        ++assets_count;
        if(assets_count > assets_capacity) {
            assets_capacity += 512;
            assets = realloc(assets, assets_capacity * sizeof(struct Asset));
        }

        /* load in asset data */
        char filename[2048];
        snprintf(filename,  2048, "build/__temp__/%s.bin", section);
        if(!file_exists(filename)) {
            fprintf(stderr, "\n * Cannot find file for asset: %s\n", section);
            exit(1);
        }

        asset->size = file_size(filename);
        asset->data = load_file(filename, asset->size);
        snprintf(asset->name, NAME_SIZE, "%s", section);

        printf("Read asset: %s\n", section);
    }

    /* note down the type */
    if(strcmp(name, "type") == 0) {
        snprintf(asset->type, NAME_SIZE, "%s", value);
    }

    /* remember the asset name */
    snprintf((char *)user, NAME_SIZE, "%s", section);
}

struct Asset *find_asset(const char *name)
{
    for(int i = 0; i < assets_count; ++i) {
        if(strcmp(assets[i].name, name) == 0) {
            return assets + i;
        }
    }

    return NULL;
}

void generate_header_for(struct Pak *pak)
{
    const char *pak_format =
    "\n/*\n"
    " * %s.dat *\n"
    "*/\n"
    "%s"
    "/* *********** */\n";

    char pak_gen[10024];

    const size_t asset_line_size = NAME_SIZE * 4;
    const size_t assets_gen_size = pak->count * asset_line_size + 512;
    char *assets_gen = calloc(assets_gen_size, sizeof(char));

    for(int i = 0; i < pak->count; ++i) {
        char asset_line[asset_line_size];

        snprintf(asset_line, asset_line_size, "%s_%s_%s %d\n", pak->name, pak->assets[i]->name, pak->assets[i]->type, pak->offsets[i]);
        for(int c = 0; c < strlen(asset_line); ++c) {
            asset_line[c] = toupper(asset_line[c]);
        }

        strncat(assets_gen, "#define ", asset_line_size - 1);
        strncat(assets_gen, asset_line, asset_line_size - 1);
    }

    snprintf(pak_gen, 10024, pak_format, pak->name, assets_gen);
    strncat(header_gen, pak_gen, HEADER_BUF_SIZE - 1);

    free(assets_gen);
}

void write_header(void)
{
    const char *header_format =
    "/*\n"
    " * ** HustleEngine AUTOMATICALLY GENERATED ** \n"
    " * \n"
    " * Offsets for assets stored in .dat pak files\n"
    "*/\n"
    "#ifndef ASSETS_GENERATED_H\n"
    "#define ASSETS_GENERATED_H\n"
    "\n%s\n"
    "#endif /* ASSETS_GENERATED_H */\n";

    char *header_all = malloc(HEADER_BUF_SIZE * 2);
    snprintf(header_all, HEADER_BUF_SIZE * 2, header_format, header_gen);

    const char *filename = "build/__temp__/assets.gen.h";
    write_out(header_all, strlen(header_all), filename, false);

    printf("Wrote header -> %s\n", filename);
}

void flush_pak(struct Pak *pak)
{
    char filename[2048];
    //snprintf(filename, 2048, "build/%s/%s.dat", platform, pak->name);
    snprintf(filename, 2048, "RES/%s.dat", pak->name);

    write_out(pak->data, pak->size, filename, true);
    printf("Wrote pak: %s -> %s (%d bytes)\n", pak->name, filename, pak->size);
    generate_header_for(pak);

    pak->count = 0;
    pak->size = 0;
}

int pak_handler(void *user, const char *section, const char *name, const char *value)
{
    struct Pak *pak = user;

    /* if this is a different pak then write it out (unless it's the first one) */
    if(strcmp(section, pak->name) != 0 && pak->name[0] != 0) {
        flush_pak(pak);
    }

    snprintf(pak->name, NAME_SIZE, "%s", section);

    /* copy asset into pak */
    struct Asset *asset = find_asset(name);
    if(!asset) {
        fprintf(stderr, "\n * Missing asset: %s\n", name);
        exit(1);
    }

    pak->count++;
    if(pak->count > MAX_ASSETS) {
        fprintf(stderr, "\n * Too many assets in pak: %s\n", section);
        exit(1);
    }

    pak->offsets[pak->count - 1] = pak->size;    /* the previous size is the current offset */

    pak->size += asset->size;
    if(pak->size > PAK_SIZE) {
        fprintf(stderr, "\n * Pak size overrun %d/%d\n", pak->size, PAK_SIZE);
        exit(1);
    }

    memcpy(pak->data + pak->offsets[pak->count - 1], asset->data, asset->size);
    pak->assets[pak->count - 1] = asset;
}

void help(void)
{
    puts("HustleEngine mkpak utility\n\n"
         "Usage: mkpak [target platform] [root dir]\n");
}

int main(int argc, char **argv)
{
    if(argc != 3) {
        help();
        return 1;
    }

    platform = argv[1];
    printf("Creating pak files for platform: %s\n\n", platform);

    const char *root_dir = argv[2];
    assert(chdir(root_dir) == 0);

    /* load assets */
    assets_capacity = 512;
    assets = malloc(assets_capacity * sizeof(struct Asset));
    char last_asset[NAME_SIZE];

    if(ini_parse("config/assets.ini", asset_handler, &last_asset[0]) < 0) {
        fprintf(stderr, "\n * Can't correctly load assets.ini\n");
        return 1;
    }

    /* pak the assets */
    struct Pak pak = {0};
    pak.data = malloc(PAK_SIZE);
    pak.offsets = malloc(MAX_ASSETS * sizeof(*pak.offsets));
    pak.assets = malloc(MAX_ASSETS * sizeof(struct Asset *));

    if(ini_parse("config/pak.ini", pak_handler, &pak) < 0) {
        fprintf(stderr, "\n * Can't correctly load pak.ini\n");
        return 1;
    }

    /* write out generated header file and the last pak */
    if(pak.name[0] != 0) {  /* may not have had any paks at all */
        flush_pak(&pak);
        write_header();
    }


    /* free everything */
    for(int i = 0; i < assets_count; ++i) {
        free(assets[i].data);
    }
}
