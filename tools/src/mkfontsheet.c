#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define ASCII_START 32
#define ASCII_END   127
#define ASCII_SIZE  95   /* ASCII_START - ASCII_END */

struct FontSheet {
    uint16_t font_size;
    uint8_t *letters;
};

SDL_Window   *window;
SDL_Renderer *renderer;
SDL_Surface  *textsurf;
SDL_Texture  *preview;
TTF_Font     *font;
SDL_Color     color = {255, 255, 255};

struct FontSheet sheet;

void quit(void)
{
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    exit(0);
}

void help(void)
{
    printf("Makes a HustleEngine font spritesheet\n");
    printf("    usage: mkfontsheet SIZE FONT OUTPUT\n\n");
}

void initialize(void)
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL2 Initialization Failed \n\n%s\n", SDL_GetError());
        exit(1);
    }
    
    TTF_Init();
    
    window = SDL_CreateWindow("HustleEngine", SDL_WINDOWPOS_UNDEFINED, 
                                              SDL_WINDOWPOS_UNDEFINED,
                                              sheet.font_size, sheet.font_size,
                                              SDL_WINDOW_SHOWN);
    if(!window) {
        fprintf(stderr, "Failed to create SDL_Window: %s\n", SDL_GetError());
        exit(1);
    }
    
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) {
        fprintf(stderr, "Failed to create SDL_Renderer %s\n", SDL_GetError());
    }
}

int main(int argc, char **argv) {
    SDL_Event event;
    char *outpath;
    
    if(argc != 4) {
        help();
        return 1;
    }
    
    /* parse input */
    sheet.font_size = atoi(argv[1]);
    assert(sheet.font_size != 0);
    printf("%d\n", sheet.font_size);
    
    initialize();

    font = TTF_OpenFont(argv[2], sheet.font_size);
    outpath = argv[3];
    
    textsurf = TTF_RenderUTF8_Solid(font, "f", color);
    preview = SDL_CreateTextureFromSurface(renderer, textsurf);
    
    /* start the copying */

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, preview, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Surface *temp = SDL_CreateRGBSurface(0, sheet.font_size, sheet.font_size, 8, 0,0,0,0);
    SDL_SetSurfaceRLE(temp, 0);
    SDL_BlitSurface(textsurf, NULL, temp, NULL);
    uint8_t *pixels = temp->pixels;    

    printf("Bits per pixel: %d Bytes per pixel: %d\n", textsurf->format->BitsPerPixel, textsurf->format->BytesPerPixel);
    printf("Dimensions: (%d %d) Pitch: %d\n", textsurf->w, textsurf->h, textsurf->pitch);

    for(int y = 0; y < sheet.font_size * 4; ++y) {
        for(int x = 0; x < sheet.font_size; ++x) {
            printf("%d", pixels[x + y]);
        }
        printf("\n");
    }
    
    SDL_Quit();
    
    return 0;
}
