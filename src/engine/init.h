#ifndef INIT_H
#define INIT_H

struct PlatformData *pd;

typedef void(*init_func_t)(struct PlatformData *pd);

void engine_init(struct PlatformData *pd);
void engine_quit(struct PlatformData *pd);

void renderer_init(struct PlatformData *pd);
void renderer_quit(struct PlatformData *pd);

#endif
