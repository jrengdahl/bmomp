// A test program for experiments with OpenMP.

//  omp9.cpp tests
//  omp_lock_t and its operators

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


#define NBUCKETS 40
#define RES 1000000

int hist[NBUCKETS];
omp_lock_t locks[NBUCKETS];

void test()
    {
    printf("hello, world!\n");

    #pragma omp parallel for
    for(int i=0;i<NBUCKETS; i++)
        {
        hist[i] = 0;
        omp_init_lock(&locks[i]);
        }

    #pragma omp parallel for
    for(int i=0; i<RES; i++)
        {
        double x;
        double y;
        int ind;

        x = i*2*M_PI/RES;
        y = sin(x);
        ind = (int)((y+1)*NBUCKETS/2);
        if(ind>=0 && ind<NBUCKETS)    
            {
            omp_set_lock(&locks[ind]);
            hist[ind]++;
            omp_unset_lock(&locks[ind]);
            }
        }

    for(int i=0; i<NBUCKETS; i++)
        {
        printf("%3d %3d\n", i, hist[i]);
        omp_destroy_lock(&locks[i]);
        }
    }


