#include "common/platform.h"
#include <string.h>
#include <stdlib.h>

void simpletest_start(void);
void test_start(bool do_benchmark, int benchmark_times);
void test_keyboard(void);
int test_mem(void);
int rletest_start(void);

void help(void)
{
    printf("Here is the list of tests:\n test\n test-keyboard\n simpletest\n benchmark\n mem\n rle\n");
}

int main(int argc, char **argv)
{
    if(argc == 2) {
        if(0 == strcmp(argv[1], "test"))
            test_start(0, 0);
        if(0 == strcmp(argv[1], "test-keyboard"))
            test_keyboard();
        if(0 == strcmp(argv[1], "simpletest"))
            simpletest_start();
        if(0 == strcmp(argv[1], "mem"))
            test_mem();
        if(0 == strcmp(argv[1], "rle"))
            rletest_start();
        else
            help();
    }
    else if(argc == 3) {
        if(0 == strcmp(argv[1], "benchmark"))
            test_start(1, atoi(argv[2]));
        else
            help();
    }
    else {
        help();
    }

    return 0;
}
