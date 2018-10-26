#ifndef KB_H
#define KB_H

#include "common/platform.h"

#ifdef PLATFORM_DOS
	#include "platform/dos/kb_dos.h"
#endif
#ifdef PLATFORM_SDL
	#include "platform/sdl/kb_sdl.h"
#endif

/*
 * Array of keys currently held down
 *
 * Can index into this with keycodes, true if key down
*/
extern const bool *keyboard_keys;

/*
 * Must call this to initialize the keyboard system
*/
void keyboard_init(void);

/*
 * Must call this on shutdown, otherwise certain platforms (DOS) may be unresponsive
*/
void keyboard_quit(void);

/*
 * Call before checking keys
*/
void keyboard_per_frame_update(void);

/*
 * Whether an OS quit event prompts the application to quit
 *
 * (Not used in DOS, mostly for SDL)
*/
bool keyboard_os_quit_event(void);

#endif /* KB_H */
