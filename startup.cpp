// A startup program that gets ready and calls main.


#include <stdint.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>

#include "exports.hpp"
#include "thread.hpp"
#include "threadFIFO.hpp"
#include "libgomp.hpp"


#define CPACR (*(uint32_t volatile *)0xE000ED88)
#define CPACR_VFPEN 0x00F00000


// The DeferFIFO used by yield, for rudimentary time-slicing.
// Note that the only form of "time-slicing" occurs when a thread
// voluntarily calls "yield" to temporarily give up the CPU
// to other threads.

threadFIFO<DEFER_FIFO_DEPTH> DeferFIFO;

extern "C"
int main(int argc, char **argv);

// for passing argc, argv, and retval between the thread and main
static void test_thread();
static int  g_argc;
static char **g_argv;
static int g_retval;


// the stack for the main thread
char stack[2048];


// this is the entry point
// we want it to be located at the beginning of the binary

extern "C"
int start(int argc, char *const argv[])
    {
    app_startup(argv);                              // init the u-boot API
    Thread::init();                                 // init the bare metal threading system
    libgomp_init();                                 // init OpenMP

    CPACR |= CPACR_VFPEN;                           // enable the floating point coprocessor

    g_argc = argc;
    g_argv = (char **)argv;

    Thread::spawn(test_thread, stack);              // spawn the test thread

    // The background loop.
    // Loop until the test thread terminates.
    // Note that this code will only spin if
    // -- all threads have yielded or suspended, or
    // -- the test thread has terminated.
    while(!Thread::done(stack))                     // while the test thread is stil running
        {
        undefer();                                  // keep trying to wake threads
        }

    printf("test complete, %d\n", g_retval);

    return (0);
    }


void test_thread()
    {
    g_retval = main(g_argc, g_argv); 
    }


