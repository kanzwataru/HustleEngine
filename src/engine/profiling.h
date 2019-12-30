#ifndef PROFILING_H
#define PROFILING_H

#include "common/platform.h"

#define HE_PROFILING /* NOTE: temp */

#ifdef HE_PROFILING
    #ifdef HE_PLATFORM_SDL2
        #include "extern/tracy/TracyC.h"
    #else
        #define TracyCZoneN(_a, _b, _c)
        #define TracyCZoneEnd(_a)
        #define TracyCFrameMark
    #endif

    struct SectionContext {
        size_t id;
        const char *name;
        const char *line;
        const char *file;
        const char *func_name;
    };

    #define PROFILE_FRAME_BEGIN() _profiling_frame_start();
    #define PROFILE_FRAME_END() \
        _profiling_frame_end(); \
        TracyCFrameMark

    #define PROFILE_SECTION_BEGIN(_name) \
        struct SectionContext _profile_scoped_ctx_##_name = {    \
            0, STRINGIFY(_name), STRINGIFY(__LINE__), __FILE__, __FUNCTION__   \
        };                                               \
        _profiling_section_begin(&_profile_scoped_ctx_##_name);  \
        TracyCZoneN(_tracy_ctx_##_name, STRINGIFY(_name), 1);

    #define PROFILE_SECTION_END(_name) \
        _profiling_section_end(&_profile_scoped_ctx_##_name); \
        TracyCZoneEnd(_tracy_ctx_##_name);

    void _profiling_frame_start(void);
    void _profiling_section_begin(struct SectionContext *ctx);
    void _profiling_section_end(struct SectionContext *ctx);
    void _profiling_frame_end(void);
#else
    #define PROFILE_FRAME_BEGIN()
    #define PROFILE_FRAME_END()
    #define PROFILE_SECTION_BEGIN(_name)
    #define PROFILE_SECTION_END(_name)
#endif

#endif
