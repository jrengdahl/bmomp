#include <stdio.h>
#include <time.h>
#include "exports.hpp"

#define MAX 1000

unsigned long record[MAX];

int main()
    {
    printf("sizeof_clock_t = %u\n", sizeof(clock_t));
    printf("CLOCKS_PER_SEC = %u\n", CLOCKS_PER_SEC);

    for(int i=0; i<MAX; i++)
        {
        record[i] = clock();
        }

    unsigned long last = 0;
    for(int i=0; i<MAX; i++)
        {
        if(record[i] != last)
            {
            printf("%6d %lu\n", i, record[i] - last);
            last = record[i]; 
            }
        }

    return 0;
    }
