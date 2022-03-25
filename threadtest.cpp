#include <stdint.h>
#include <stdio.h>

#include "exports.hpp"
#include "thread.hpp"
#include "threadFIFO.hpp"

Thread x;

char stack[1024] __attribute__((aligned(8)));

threadFIFO<DEFER_FIFO_DEPTH> DeferFIFO;

void thread();

extern "C"
int start(int argc, char *const argv[])
    {
    app_startup(argv);
    Thread::init();

    printf("hello, world!\n");
    printf("main: spawning thread\n");
    Thread::spawn(thread, stack);
    printf("main: back from spawn, done flag = %u\n", Thread::done(stack));
    printf("main: trying resume next\n");
    x.resume();
    printf("main: back from resume, trying undefer next (twice)\n");
    undefer();
    printf("main: back from first undefer, done flag = %u\n", Thread::done(stack));
    undefer();
    printf("main: back from second undefer, done flag = %u\n", Thread::done(stack));
    printf("main: test complete\n");

    return (0);
    }


void thread()
    {
    printf("thread: started, trying suspend next\n");
    x.suspend();
    printf("thread: back from suspend, trying yield next\n");
    yield();
    printf("thread: back from yield, exiting\n");
    }


