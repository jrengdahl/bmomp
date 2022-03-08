#include <stdint.h>

#include "exports.hpp"
#include "thread.hpp"
#include "threadFIFO.hpp"

Thread x;

char stack[128] __attribute__((aligned(8)));

threadFIFO<16> DeferFIFO;

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
    printf("main: returned from resume, trying undefer next\n");
    undefer();
    undefer();
    printf("main: thread done = %u\n", Thread::done(stack));
    printf("main: test complete\n");

    return (0);
    }


void thread()
    {
    printf("thread: started\n");
    x.suspend();
    printf("thread: resumed\n");
    yield();
    printf("thread: returned from yield, exiting\n");
    }


