#include "test.h"
#include "simplet.h"

#include <string.h>
#include <stdlib.h>

void help(void)
{
    printf("Here is the list of tests: test\n test-keyboard\n simpletest\n benchmark\n \n");
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
    }
    else if(argc == 3) {
        if(0 == strcmp(argv[1], "benchmark"))
            test_start(1, atoi(argv[2]));
    }
    else {
        help();
    }

    return 0;
}
