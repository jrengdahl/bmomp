#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <math.h>


#define NBUCKETS 40
#define RES 1000000

int hist[NBUCKETS];
omp_lock_t locks[NBUCKETS];

int main()
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
        int index;

        x = i*2*M_PI/RES;
        y = sin(x);
        index = (int)((y+1)*NBUCKETS/2);
        if(index>=0 && index<NBUCKETS)    
            {
            omp_set_lock(&locks[index]);
            hist[index]++;
            omp_unset_lock(&locks[index]);
            }
        }

    for(int i=0; i<NBUCKETS; i++)
        {
        printf("%3d %3d\n", i, hist[i]);
        omp_destroy_lock(&locks[i]);
        }


    return 0;
    }


