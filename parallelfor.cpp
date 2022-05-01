// A test program for experiments with OpenMP.

//  test "#pragma omp parallel for"

#include <stdint.h>
#include <stdio.h>
#include <omp.h>
#include "exports.hpp"

#define LIMIT 32

int main()
    {
    unsigned array[LIMIT];

    printf("hello, world!\n");

    #pragma omp parallel for num_threads(LIMIT)
    for(int i=0; i<LIMIT; i++)
        {
        array[i] = omp_get_thread_num();                // fill an array with the number of the thread that processed each element of the array
        }

    for(int i=0; i<LIMIT; i++)
        {
        printf("%u ", array[i]);                        // print the array
        }
    printf("\n");

    return 0;
    }


