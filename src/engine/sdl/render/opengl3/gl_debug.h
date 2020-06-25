#ifndef GL_DEBUG_H
#define GL_DEBUG_H

#include "common/platform.h"

#define check_gl_error()    _check_gl_error(__FILE__, __LINE__)
__attribute__((unused)) static int _check_gl_error(const char *file, int line)
{
    GLenum err = 0;
    while((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "%s:%d -> OpenGL Error: %04x\n", file, line, err);
    }

    return err == 0;
}

#endif
