#ifndef HUSTLE_DEBUG_H
#define HUSTLE_DEBUG_H

#define PANIC_START() debug_panic();
#define PANIC_DO(x) do { debug_panic(); x; debug_terminate(); } while(0);
#define PANIC(msg) PANIC_DO(fprintf(stderr, "!! %s (%s %d)\n", (msg), __FILE__, __LINE__));

#ifdef DEBUG
    #define NOT_IMPLEMENTED \
        PANIC("Not implemented");    

    #define assert(expr) \
        if(expr) {} else { PANIC_DO(printf("Assert fail %s, %d\n", __FILE__, __LINE__)) }

    #define DEBUG_DO(x) x;
#else /* if not debug */
    #define NOT_IMPLEMENTED /* disabled */
    #define assert(expr)    /* disabled */
    #define DEBUG_DO(x)     /* disabled */
#endif

void debug_panic(void);      /* go into panic-mode (on DOS this means text-mode) */
void debug_terminate(void);  /* quit the program imediately */

#endif
