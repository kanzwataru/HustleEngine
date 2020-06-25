#include "common/platform.h"
#include "platform/bootstrap.h"
#include "engine/init.h"
#include "nativeplatform.h"

#include <stdlib.h>
#include <stdio.h>

#define GAME_LIB "./game.running." STRINGIFY(HE_LIB_EXT)
#define GAME_ORIG_LIB "./game." STRINGIFY(HE_LIB_EXT)

static struct PlatformData platform = {0};
static struct Game game_table;
static bool sdl_loaded = false;
static void *lib_handle = NULL;
static void *memory = NULL;

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
        SDL_UnloadObject(lib_handle);
    }

    if(system("cp " GAME_ORIG_LIB " " GAME_LIB) != 0) {
        warn("Failed to copy game library");
        return false;
    }

    lib_handle = SDL_LoadObject(GAME_LIB);
    if(!lib_handle) {
        warn(SDL_GetError());
        return false;
    }

    game_handshake_func_t handshake = SDL_LoadFunction(lib_handle, STRINGIFY(HANDSHAKE_FUNCTION_NAME));
    if(!handshake) {
        warn(SDL_GetError());
        return false;
    }

    init_func_t engine_init = SDL_LoadFunction(lib_handle, "engine_init");

    handshake(&game_table, memory);
    engine_init(&platform);

    return true;
}

static void recompile(void)
{
    puts("*** game library recompile ***");

    if(system("cd " STRINGIFY(HE_MAKE_DIR) " && make -j8 --no-print-directory") != 0) {
        fprintf(stderr, "*** game library compilation failed ***");
        return;
    }

    puts("*** game library compiled ***");

    load_game();
}

static void sdl_init(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Quit();
        err(SDL_GetError());
    }

    sdl_loaded = true;

    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    platform.window_handle = SDL_CreateWindow(
        STRINGIFY(HE_GAME_NAME),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        platform.screen_size.w * 2, platform.screen_size.h * 2,
        //SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP
        SDL_WINDOW_OPENGL
    );

    int w, h;
    SDL_GetWindowSize(platform.window_handle, &w, &h);
    platform.screen_size = (Rect){0, 0, w, h};
    printf("%d %d\n", platform.screen_size.w, platform.screen_size.h);

    platform.gl_context = SDL_GL_CreateContext(platform.window_handle);

    SDL_GL_SetSwapInterval(1);
}

static void sdl_quit(void)
{
    if(platform.gl_context) {
        SDL_GL_DeleteContext(platform.gl_context);
    }

    if(platform.window_handle) {
        SDL_DestroyWindow(platform.window_handle);
    }

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
            if(e.key.keysym.sym == SDLK_ESCAPE)
                return false;
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

        //SDL_GL_SwapWindow(platform.window_handle);
    }

    game_table.quit();
}

static void cleanup(void)
{
    init_func_t engine_quit = SDL_LoadFunction(lib_handle, "engine_quit");
    engine_quit(&platform);

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

    memory = malloc(33000000);
    if(!memory)
        err("Could not allocate memory");

    platform.screen_size = (Rect){0, 0, WIDTH * 2, HEIGHT * 2};
    platform.target_size = (Rect){0, 0, WIDTH, HEIGHT};

    sdl_init();
    if(!load_game())
        err("Could not load game " GAME_LIB);

    game_loop();

    return 0;
}
