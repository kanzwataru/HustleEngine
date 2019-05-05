#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "extern/inih/ini.h"

#define SECTION_MAX 32000
char last_section[SECTION_MAX];

void help(void)
{
    fprintf(stderr, "HustleEngine lsini tool\n"
                    "lists all sections in an ini file\n\n"
                    "usage: lsini <ini file>\n");
}

int print_sections(void *user, const char *section, const char *name, const char *value)
{
    if(strcmp(section, last_section) != 0) {
        snprintf(last_section, SECTION_MAX, "%s", section);
        fprintf(stdout, "%s\n", section);
    }

    return 1;
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        help();
        return 1;
    }

    if(ini_parse(argv[1], print_sections, NULL)) {
        fprintf(stderr, "INI file error for: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
