// A test program for experiments with OpenMP.

//  omp7.cpp tests
//  "#pragma omp single"

#include <stdint.h>
#include <stdio.h>

#include "exports.hpp"
#include "thread.hpp"
#include "threadFIFO.hpp"
#include "libgomp.hpp"
#include "omp.h"


#define CPACR (*(uint32_t volatile *)0xE000ED88)
#define CPACR_VFPEN 0x00F00000


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


int arg = 0;

extern "C"
int start(int argc, char *const argv[])
    {
    app_startup(argv);                              // init the u-boot API
    Thread::init();                                 // init the bare metal threading system
    libgomp_init();                                 // init OpenMP

    if(argc==1)arg = 0;
    else arg = argv[1][0]&7;

    CPACR |= CPACR_VFPEN;                           // enable the floating point coprocessor

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


void spin(int x)
    {
    while(x--)
        {
        yield();
        }
    }


void stuff(int id, char c, long count)
    {
    printf("begin section %c = %d (%ld)\n", c, id, count);
    spin(count);
    printf("end   section %c = %d (%ld)\n", c, id, count);
    }


void test()
    {
    printf("hello, world!\n");

    #pragma omp parallel num_threads(5)
        {
        int id = omp_get_thread_num();

        printf("id1 = %d\n", id);

        #pragma omp sections
            {
            #pragma omp section
                {
                stuff(id, 'A', 8000000);
                }

            #pragma omp section
                {
                stuff(id, 'B', 1000000);
                }

            #pragma omp section
                {
                stuff(id, 'C', 7000000);
                }

            #pragma omp section
                {
                stuff(id, 'D', 2000000);
                }

            #pragma omp section
                {
                stuff(id, 'E', 6000000);
                }

            #pragma omp section
                {
                stuff(id, 'F', 3000000);
                }

            #pragma omp section
                {
                stuff(id, 'G', 5000000);
                }

            #pragma omp section
                {
                stuff(id, 'H', 4000000);
                }
            }

        printf("id2 = %d\n", id);
        }
    }


