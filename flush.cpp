// test #pragma omp flush
// using a producer and consumer

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <math.h>
#include "Compiler.hpp"


#define N 16

int A[N];
int flag = 0;

int B[N];


int main()
    {
    for(int j=0; j<100; j++)
    {
    #pragma omp parallel
        {
        #pragma omp sections                                    // there are only two sections
            {

            #pragma omp section                                 // the first section is the producer
                {
                for(int i=0; i<N; i++)                          // produce N whatsits
                    {
                    while(flag==1){ COMPILE_BARRIER; }          // wait for the consumer to take the previous whatsit
                    #pragma omp flush                           // barrier: don't start filling the array until the last one has been taken
                    for(int k=0; k<N; k++)                      // fill the array with stuff
                        {
                        A[k] = i+k;
                        }
                    #pragma omp flush                           // barrier, don't hand off the array until it has been filled
                    flag = 1;                                   // hand off the array to the consumer
                    }
                }

            #pragma omp section                                 // the second section is the consumer
                {
                for(int i=0; i<N; i++)                          // consumer N whatsits
                    {
                    int sum;
                    while(flag==0){ COMPILE_BARRIER; }          // wait until the consumer says go
                    #pragma omp flush                           // barrier, don;t touch the array until the flag is set
                    sum = 0;
                    for(int k=0; k<N; k++)                      // sum everything in the array
                        {
                        sum += A[k];
                        }
                    B[i] = sum;                                 // put the sum in an array of results
                    #pragma omp flush                           // barrier, don't release the array until the summation is complete
                    flag = 0;                                   // tell the producer that we're done
                    }
                }            

            }        
        }

    for(int i=0; i<N; i++)                                      // print the array of results
        {
        printf("%4d", B[i]);
        }
    printf("\n");
    }

    return 0;
    }


