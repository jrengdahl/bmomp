//  test "#pragma omp barrier"

#include <stdint.h>
#include <stdio.h>
#include <omp.h>
#include "exports.hpp"

#define LIMIT 32

unsigned A[LIMIT];
unsigned B[LIMIT];

int main()
    {
    printf("hello, world!\n");

    #pragma omp parallel num_threads(LIMIT)
        {
        int i = omp_get_thread_num();
        A[i] = i;                                   // fill each element of A with the number of the thread that processed that element
        #pragma omp barrier                         // wait until all threads have done so
        B[i] = A[(i+1)%LIMIT];                      // copy A[i+1] to B[i]
        }

    for(int i=0; i<LIMIT; i++)
        {
        printf("%2u ", A[i]);                       // print A
        }
    printf("\n");

    for(int i=0; i<LIMIT; i++)
        {
        printf("%2u ", B[i]);                        // print B
        }
    printf("\n");

    return 0;
    }


