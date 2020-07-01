#ifndef RENDER_PRESENT_H
#define RENDER_PRESENT_H

#include "nativeplatform.h"

void display_init(struct PlatformData *pd);
void display_quit(struct PlatformData *pd);
void display_present(struct PlatformData *pd);
void display_swap_buffers(struct PlatformData *pd);

#endif /* end of include guard: RENDER_PRESENT_H */
