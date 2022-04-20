/*
 *  calibrate.cpp
 *
 * Verify the accuracy of the CPU clock.
 *
 * This program spins for a defined number of CPU clocks, then terminates.
 * It uses systick to measure the time it took, and prints the result.
 * You should also use a stop watch to double check the accuracy of the systick and CPU clock.
 * This code assumes that the inner loop can execute both the decrement and loop in one clock per iteration.
 * This was originally designed for the Cortex-M7, which does support dual-issue. If you port this to
 * another CPU (e.g. a Cortex-M4) you will have to change the inner loop increment to the number of
 * clocks per iteration.
 *
 * Arg: the number of CPU clocks to delay (a long long).
 *
 * If the CPU clock is 480,000,000 MHz, you should specify at least 4,800,000,000 ticks (10 seconds)
 * for a accurate result.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "exports.hpp"

int main(int argc, char **argv)
    {
    unsigned long long ticks = 480000000;
    unsigned long start;
    unsigned long elapsed;
    unsigned tl;
    unsigned th;

    if(argc > 1)
        {
        ticks = atoll(argv[1]);
        }

    printf("ticks = %08x%08x (%d)\n", (unsigned)(ticks>>32), (unsigned)ticks, sizeof(ticks)); 

    tl = (unsigned)ticks;
    th = (unsigned)(ticks>>32);

    start = get_timer(0);

    __asm__ __volatile__(
        "cbz %[tl], 2f        \n"
        "1: subs %[tl], #1    \n\t"
        "bne 1b               \n"
        "2: cbz %[th], 3f     \n\t"
        "subs %[th], #1       \n\t"
        "b 1b                 \n"
        "3:                   \n"
        :[tl]"+r"(tl), [th]"+r"(th)
        :
        :);

    elapsed = get_timer(start);

    printf("elapsed time = %ld.%03lu\n", elapsed/1000, elapsed%1000); 

    return 0;
    }
