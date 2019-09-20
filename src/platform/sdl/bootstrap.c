#include "common/platform.h"
#include "platform/bootstrap.h"
#include "nativeplatform.h"

#include <stdlib.h>
#include <stdio.h>

#define GAME_LIB "./game." STRINGIFY(HE_LIB_EXT)

static struct PlatformData platform_data = {0};
static struct Game game_table;
static bool sdl_loaded = false;
static void *lib_handle = NULL;

static void warn(const char *err_msg)
{
    fprintf(stderr, "\nBootstrap Error: %s\n", err_msg);

    if(sdl_loaded) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Hustle Engine Bootstrap Error", err_msg, NULL);
    }
}

static void err(const char *err_msg)
{
    warn(err_msg);

    exit(1);
}

static bool load_game(void)
{
    if(lib_handle) {
        SDL_UnloadObject(GAME_LIB);
    }

    lib_handle = SDL_LoadObject(GAME_LIB);
    if(!lib_handle) {
        warn(SDL_GetError());
        return false;
    }

    GameHandshadeFunction handshake = SDL_LoadFunction(lib_handle, STRINGIFY(HANDSHAKE_FUNCTION_NAME));
    if(!handshake) {
        warn(SDL_GetError());
        return false;
    }

    handshake(&game_table, NULL);

    return true;
}

static void recompile(void)
{

}

static void sdl_init(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Quit();
        err(SDL_GetError());
    }

    sdl_loaded = true;

    platform_data.window = SDL_CreateWindow(
        STRINGIFY(HE_GAME_NAME),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        320, 200,
        0
    );

    platform_data.renderer = SDL_CreateRenderer(platform_data.window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
}

static void sdl_quit(void)
{
    if(platform_data.window)
        SDL_DestroyWindow(platform_data.window);

    SDL_Quit();

    sdl_loaded = false;
}

bool sdl_handle_events(void) {
    SDL_Event e;

    while(SDL_PollEvent(&e) != 0) {
        switch(e.type) {
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:
            if(e.key.keysym.sym == SDLK_F5)
                recompile();
            break;
        }
    }

    return true;
}

static void game_loop(void)
{
    game_table.running = true;
    game_table.init();

    while(sdl_handle_events() && game_table.running) {
        //ensure_game_latest();

        game_table.input();
        game_table.update();
        game_table.render();
    }

    game_table.quit();
}

static void cleanup(void)
{
    if(lib_handle && sdl_loaded) {
        SDL_UnloadObject(lib_handle);
    }

    if(sdl_loaded) {
        sdl_quit();
    }
}

int main(int argc, char **argv)
{
    atexit(cleanup);

    sdl_init();
    if(!load_game())
        err("Could not load game " GAME_LIB);

    game_loop();

    return 0;
}
