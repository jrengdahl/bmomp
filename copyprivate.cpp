// A test program for experiments with OpenMP.

//  copyprivate.cpp tests
//  #pragma omp single copyprivate


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

int main(int argc, char **argv)
    {
    int var;

    #pragma omp parallel private(var)
        {
        #pragma omp single copyprivate(var)
            {
            if(argc>1)var = atoi(argv[1]);
            else var = 0;

            printf("%d\n", omp_get_thread_num());
            }

        printf("%d=%d\n", omp_get_thread_num(), var);
        }
    return 0;
    }


