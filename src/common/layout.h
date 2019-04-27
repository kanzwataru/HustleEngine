/*
 * HustleEngine memory map
*/
#ifndef LAYOUT_H
#define LAYOUT_H

#define MEMSLOT_RENDERER_BACKBUFFER    0 /* screen back buffer segment */
#define MEMSLOT_RENDERER_BG            1 /* background layer segment, NOTE: Only used if RENDER_BG_SOLID flag is not set */
#define MEMSLOT_RENDERER_TRANSIENT     2 /* renderer scratch data segment (unused by default) */
#define MEMSLOT_MEMORY_POOL            5 /* dynamic allocator pool */

#endif
