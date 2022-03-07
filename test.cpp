#include <common.h>
#include <exports.h>
#include <omp.h>

Thread x;

char stack[128] __attribute__((__aligned(8)));

void thread()
    {
    printf("thread: started\n");
    x.suspend();
    printf("thread: resumed, exiting\n");
    }


extern "C"
int hello(int argc, char *const argv[])
    {
    app_startup(argv);

    printf("hello, world!\n");
    start(thread, stack);
    printf("main: thread started %d\n");
    x.resume();
    printf("main: done\n");

    return (0);
    }
