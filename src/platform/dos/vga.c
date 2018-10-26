#include "platform/video.h"
#include "common/platform.h"
#include <dos.h>

#define PALETTE_INDEX       0x03c8 /* vga palette register */
#define PALETTE_DATA        0x03c9
#define INPUT_STATUS_0      0x03da /* used for querying vblank */

enum VGA_VIDEO_MODES {
    VGA_CHUNKY256 = 0x13, /* 256 colour indexed chained,  320x200 non-square pixels */
    VGA_PLANAR16  = 0x12, /* 16  colour planar unchained, 640x400 square pixels */
    VGA_PLANAR16HALF = 0x12, /* 16 colour planar unchained, 640x200 non-square (2:1) pixels */
    VGA_TEXT80COL = 0x03  /* 80 column text mode, 80x25 */
};

#define VGA_CHUNKY256_SIZE      320 * 200
#define VGA_PLANAR16_SIZE       640 * 480
#define VGA_PLANAR16HALF_SIZE   640 * 200

static buffer_t *vga_mem = MK_FP(0xa000, 0); /* VGA DMA memory */

static byte current_mode = 0;
static uint16 screen_size = 0;

/*
 * Switch VGA display mode
 * 
 * (uses VGA_VIDEO_MODES not video.h VIDEO_MODES)
*/
static void vga_modeset(byte mode)
{
    union REGS in, out;

    in.h.ah = 0;
    in.h.al = mode;
    int86(0x10, &in, &out);
}

/*
 * Video API flip buffer
*/
void video_flip(buffer_t *backbuf)
{
    assert(current_mode != 0);
    assert(screen_size != 0);
    
    /* we ONLY support Mode 13h for now!! */
    if(current_mode != VGA_CHUNKY256)
        NOT_IMPLEMENTED;
    
    _fmemcpy(vga_mem, backbuf, screen_size);
}

/*
 * Video API init
*/
void video_init_mode(byte mode, byte scaling)
{
    /* we ONLY support Mode 13h for now!! */
    if(mode != VIDEO_MODE_LOW256)
        NOT_IMPLEMENTED;
    
    current_mode = VGA_CHUNKY256;
    screen_size  = VGA_CHUNKY256_SIZE;
    vga_modeset(VGA_CHUNKY256);
}

/*
 * Video API exit
*/
void video_exit(void)
{
    vga_modeset(VGA_TEXT80COL);
}

/*
 * Block and wait for vsync
*/
void video_wait_vsync(void)
{
    while(inportb(INPUT_STATUS_0) & 8) {;}
    while(!(inportb(INPUT_STATUS_0) & 8)) {;}
}

/*
 * Sets mode 13h
 * 256 colour palette
*/
void video_set_palette(buffer_t *palette)
{
    int i;

    outp(PALETTE_INDEX, 0);     /* tell the VGA that palette data is coming. */

    for(i = 0; i < 256 * 3; ++i)
        outp(PALETTE_DATA, palette[i]);
}
