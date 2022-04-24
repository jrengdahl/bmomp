// A test program for experiments with OpenMP.

//  omp10.cpp tests
//  #pragma omp task
//  and the firstprivate clause

// This test uses a recursive permutation generator. Watch out for stack and other resource overflows.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

// the stack for the test thread
char stack[2048];



const int COLORS = 4;               // max number of colors
const int MAX = 32;                 // max number of colors times steps
const int THREADS = 32;             // max number of threads in the team

unsigned input;                     // the number of balls of each color (an array of four bytes)
unsigned char output[MAX];          // the initial permutation output. Each task makes a copy of this.
int colors = 2;                     // the number of colors for this test
int balls;                          // the number of balls per color
int plevel = 0;                     // max level to make parallel
int permutations = 0;               // total number of permutations
int ids[THREADS+10];                // an array to record what threads ran
int maxthread = 0;                  // the highest thraed number that was spawned



// forward declaration of the test thread
// We want the test thread to be later in the program so that "start" is at the beginning of the binary,
// so we can say "go 24000000" and not have to look up the entry point in the map.
void test();

// this is the entry point
// we want it to be located at the beginning of the binary

extern "C"
int start(int argc, char *const argv[])
    {
    app_startup(argv);                              // init the u-boot API
    Thread::init();                                 // init the bare metal threading system
    libgomp_init();                                 // init OpenMP

    CPACR |= CPACR_VFPEN;                           // enable the floating point coprocessor

    printf("hello, world!\n");

    if(argc>1)colors=atoi(argv[1]);                 // first arg: the number of color
    else colors=2;

    if(argc>2)balls=atoi(argv[2]);                  // second arg: how many balls there are of each color
    else balls=2;

    if(argc>3)plevel = atoi(argv[3]);               // third arg: the number of levels to spawn new threads

    if(argc>4)omp_verbose=atoi(argv[4]);            // fourth arg, verbosity level


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


// return a color count from the input supply
inline unsigned cget(int i, unsigned input)
    {
    return (input>>(i*8)) & 255;
    }

// decrement a color in the input supply
inline unsigned cdec(int i, unsigned input)
    {
    return input - (1<<(i*8));
    }


// compute permutations of colored balls.
// There may be multiple balls of each color.
// input: the number of balls left of each color. Maximum of four colors. Each byte of the word represents one color.
// output: a reference to an array of the balls that have been chosen so far.
// step: the depth of recursion, starts at zero and increments for each level descended.
// left: the number of balls remaining to be chosen. Starts at colors*n and decrements for each recursion.

void permute(unsigned input, unsigned char (&output)[MAX], int step, int left)
    {
    int id = omp_get_thread_num();                                  // record the thread number
    ids[id] = id;                                                   // for analysis only, it's not used in the permutation calculation

    if(left == 0)                                                   // if all the balls have been chosen
        {
        if(omp_verbose>=1)                                          // if verbose, print out the permutaiton
            {
            #pragma omp critical
                {        
                for(int i=0; i<step; i++)
                    {
                    printf("%u ", output[i]);
                    }
                printf(" (%d)\n", omp_get_thread_num());
                }
            }

        #pragma omp atomic
            ++permutations;
        }
    else                                                            // if not done yet
        {
        for(int i=0; i<colors; i++)                                         // for each possible color
            {
            if(cget(i, input) > 0)                                          // if there are any left of that color
                {
                output[step] = i;                                           // pick a ball of that color
                #pragma omp task firstprivate(output) if(step < plevel)     // note that firstprivate make a copy of the output for the new task
                    {
                    permute(cdec(i, input), output, step+1, left-1);        // and recurse, deleting one ball of the chosen color from the input
                    }
                }
            }
        }
    }


void test()
    {
    if(colors > COLORS || colors*balls > MAX)           // check validity of inputs
        {
        printf("too many\n");
        }

    input = 0;                                          // init the input
    for(int i=0; i<colors; i++)
        {
        input |= balls<<(i*8);
        }

    for(auto &x : output) x = 0;                        // init the output
    for(auto &x : ids) x = 0;                           // init the ID array (used for curiosity only)

    #pragma omp parallel num_threads(THREADS)           // kick off the permuter
    #pragma omp single
    permute(input, output, 0, colors*balls);

    for(auto id: ids)maxthread = id>maxthread?id:maxthread; // figure out how many threads were used

    printf("permutations = %d\n", permutations);        // print results
    printf("maxthread = %d\n", maxthread);
    }


