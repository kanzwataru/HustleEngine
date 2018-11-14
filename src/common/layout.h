/*
 * HustleEngine memory map (TODO: make this game-specific)
*/
#ifndef LAYOUT_H
#define LAYOUT_H

#define MEMSLOT_RENDERER_BACKBUFFER    0 /* screen back buffer segment */
#define MEMSLOT_RENDERER_BG            1 /* background layer segment, NOTE: This is not used if RENDER_BG_SOLID flag is set */ 
#define MEMSLOT_RENDERER_TRANSIENT     2 /* renderer scratch data segment */
#define MEMSLOT_MEMORY_POOL            5 /* dynamic allocator pool */

#endif
