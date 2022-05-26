//  test "#pragma omp critical", , GOMP_critical_start, and GOMP_critical_end

#include <stdint.h>
#include <stdio.h>
#include <omp.h>
#include "exports.hpp"
#define LIMIT 32

unsigned A[LIMIT];
unsigned B[LIMIT];
unsigned sum = 0;

int main()
    {
    printf("hello, world!\n");

    #pragma omp parallel
        {
        int i = omp_get_thread_num();
        A[i] = i;

        #pragma omp barrier
        B[i] = A[(i+1)%LIMIT];

        #pragma omp critical
        sum += B[i];
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

    printf("sum = %u \n", sum);

    return 0;
    }


