#include "converters/converters.hpp"

namespace spritesheet {

void bmp2rle(Asset asset, std::FILE *out)
{

}

}

// /*
//  * Convert BMP file to RLE
// */
// #include "lib/aconv.h"
// #include "lib/stdinc.h"
//
// #define MAX_SIZE 1280000
//
// void help(void)
// {
//     fprintf(stderr, "bmp2rle [file] > out.bin\n");
// }
//
// int main(int argc, char **argv)
// {
//     if(argc != 2) {
//         help();
//         return 1;
//     }
//
//     struct ImageInfo info = {0};
//     size_t size;
//     uint8_t *image_data = NULL;
//     uint8_t *rle = calloc(1, MAX_SIZE);
//     assert(rle);
//
//     image_data = load_bmp_image(argv[1], &info);
//     assert(image_data);
//     assert(info.width && info.height);
//
//     size = buffer_to_rle((RLEImage *)rle, image_data, info.width, info.height);
//     assert(size <= MAX_SIZE); /* this sucks, overflows should be prevented not allowed */
//
//     fwrite(image_data, sizeof(uint8_t), size, stdout);
//
//     return 0;
// }
