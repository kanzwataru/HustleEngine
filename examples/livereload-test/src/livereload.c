#include "platform/bootstrap.h"

void init(void) {

}

bool input(void) {
    return true;
}

void update(void) {

}

void render(void) {

}

void quit(void) {

}

void HANDSHAKE_FUNCTION_NAME(struct Game *game, void *memory_chunk) {
    game->init = init;
    game->input = input;
    game->update = update;
    game->render = render;
    game->quit = quit;
}
