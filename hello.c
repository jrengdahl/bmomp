#include <common.h>
#include <exports.h>

int hello(int argc, char *const argv[])
    {
    app_startup(argv);

    printf("hello, world!\n");

    return (0);
    }
