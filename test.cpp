#include <stdint.h>

#include "exports.hpp"
#include "thread.h"

Thread x;

char stack[128] __attribute__((aligned(8)));

void thread();

extern "C"
int start(int argc, char *const argv[])
    {
    app_startup(argv);

    printf("hello, world!\n");
    Thread::spawn(thread, stack);
    printf("main: thread started\n");
    x.resume();
    printf("main: done\n");

    return (0);
    }


void thread()
    {
    printf("thread: started\n");
    x.suspend();
    printf("thread: resumed, exiting\n");
    }


