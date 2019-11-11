#pragma once

/* for program using configs */
#define config_get(_type)   _type##_assets
#define config_size(_type)  _type##_count

/* for config file */
#ifdef THIS_IS_CONFIG_FILE
#define begin(_type) \
    struct _type _##_type[] = {

#define end(_type) \
    }; \
    struct _type *_type##_assets = _##_type; \
    int _type##_count = sizeof(_##_type) / sizeof(struct _type);

#define none(_type) \
    struct _type *_type##_assets = 0; \
    int _type##_count = 0;

#endif

/* for config header */
#define config_declare(_type) \
    extern struct _type *_type##_assets; \
    extern int _type##_count;
