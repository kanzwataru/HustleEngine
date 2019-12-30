#ifndef PROFILING_H
#define PROFILING_H

#include "common/platform.h"
#include "extern/tracy/TracyC.h"

#define HE_PROFILING /* NOTE: temp */

#ifdef HE_PROFILING
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
        struct SectionContext _profile_scoped_ctx = {    \
            0, _name, STRINGIFY(__LINE__), __FILE__, __FUNCTION__   \
        };                                               \
        _profiling_section_begin(&_profile_scoped_ctx);  \
        TracyCZoneN(_tracy_ctx, _name, 1);

    #define PROFILE_SECTION_END(_name) \
        _profiling_section_end(&_profile_scoped_ctx); \
        TracyCZoneEnd(_tracy_ctx);

    //void profiling_toggle(bool enabled);
    //void profiling_toggle_display(bool enabled);

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
