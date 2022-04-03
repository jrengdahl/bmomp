#include <exports.hpp>

int hello(int argc, char *const argv[])
    {
    app_startup(argv);

    printf("hello, world!\n");

    return (0);
    }
