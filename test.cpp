#include <stdint.h>

#include "exports.hpp"
#include "thread.hpp"

Thread x;

char stack[128] __attribute__((aligned(8)));

void thread();

extern "C"
int start(int argc, char *const argv[])
    {
    app_startup(argv);
    Thread::init();

    printf("hello, world!\n");
    Thread::spawn(thread, stack);
    printf("main: thread done = %u\n", Thread::done(stack));
    x.resume();
    printf("main: thread done = %u\n", Thread::done(stack));
    printf("main: test complete\n");

    return (0);
    }


void thread()
    {
    printf("thread: started\n");
    x.suspend();
    printf("thread: resumed, exiting\n");
    }


