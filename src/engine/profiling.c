#include "profiling.h"

#define TRACY_ONLY

#ifdef HE_PROFILING
#include "engine.h"
#include <stdio.h>

struct SectionTiming {
    struct SectionContext ctx_copy;
    double milliseconds;
};

#ifndef TRACY_ONLY
#define SECTION_MAX 512
static struct SectionTiming sections[SECTION_MAX];
static size_t section_top = 0;
#endif

void _profiling_section_begin(struct SectionContext *ctx)
{
#ifndef TRACY_ONLY
    ctx->id = section_top;
    sections[ctx->id].ctx_copy = *ctx;

    ++section_top;
    assert(section_top < SECTION_MAX);

    timer_start();
#endif
}

void _profiling_section_end(struct SectionContext *ctx)
{
#ifndef TRACY_ONLY
    sections[ctx->id].milliseconds = timer_stop_get_ms();
#endif
}

void _profiling_frame_start(void)
{
#ifndef TRACY_ONLY
    section_top = 0;
#endif
}

void _profiling_frame_end(void)
{
#ifndef TRACY_ONLY
    fprintf(logdev, "\nframe timings begin\n");
    for(int i = 0; i < section_top; ++i) {
        struct SectionContext *ctx = &sections[i].ctx_copy;
        fprintf(logdev, "\tsection: %s (at %s)\n", ctx->name, ctx->line);
        fprintf(logdev, "\t\tfunction '%s' in '%s'\n", ctx->func_name, ctx->file);
        fprintf(logdev, "\t\tmilliseconds: %f\n", sections[i].milliseconds);
    }
    fprintf(logdev, "frame timings end\n");
#endif
}

#endif /* HE_PROFILING enabled */
