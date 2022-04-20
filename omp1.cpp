// A test program for experiments with OpenMP.

//  omp1.cpp tests
//  "#pragma omp parallel for"

#include <stdint.h>
#include <stdio.h>

#include "exports.hpp"
#include "thread.hpp"
#include "threadFIFO.hpp"
#include "libgomp.hpp"
#include "omp.h"

// The DeferFIFO used by yield, for rudimentary time-slicing.
// Note that the only form of "time-slicing" occurs when a thread
// voluntarily calls "yield" to temporarily give up the CPU
// to other threads.

threadFIFO<DEFER_FIFO_DEPTH> DeferFIFO;

// forward declaration of the test thread
// We want the test thread to be later in the proram so that "start" is at the beginning of the binary,
// so we can say "go 24000000" and not have to look up the entry point in the map.
void test();

// the stack for the test thread
char stack[2048];

// this is the entry point
// we want it to be located at the beginning of the binary

extern "C"
int start(int argc, char *const argv[])
    {
    app_startup(argv);                              // init the u-boot API
    Thread::init();                                 // init the bare metal threading system
    libgomp_init();                                 // init OpenMP

    Thread::spawn(test, stack);                     // spawn the test thread

    // The background loop.
    // Loop until the test thread terminates.
    // Note that this code will only spin if
    // -- all threads have yielded or suspended, or
    // -- the test thread has terminated.
    while(!Thread::done(stack))                     // while the test thread is stil running
        {
        undefer();                                  // keep trying to wake threads
        }

    printf("test complete\n");

    return (0);
    }


#define LIMIT 32

void test()
    {
    unsigned array[LIMIT];

    printf("hello, world!\n");

    #pragma omp parallel for num_threads(LIMIT)
    for(int i=0; i<LIMIT; i++)
        {
        array[i] = omp_get_thread_num();                // fill an array with the number of the thread that procsed each element of the array
        }

    for(int i=0; i<LIMIT; i++)
        {
        printf("%u ", array[i]);                        // print the array
        }
    printf("\n");
    }


