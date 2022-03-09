#include <stdint.h>
#include <stdio.h>

#include "exports.hpp"
#include "thread.hpp"
#include "threadFIFO.hpp"
#include "libgomp.hpp"
#include "omp.h"

threadFIFO<DEFER_FIFO_DEPTH> DeferFIFO;

void test();
char stack[1024];

extern "C"
int start(int argc, char *const argv[])
    {
    app_startup(argv);
    Thread::init();
    libgomp_init();

    Thread::spawn(test, stack);

    while(DeferFIFO)
        {
        undefer();
        }

    printf("test complete\n");

    return (0);
    }


#define LIMIT 32

void test()
    {
    unsigned array[LIMIT];

    printf("hello, world!\n");

    #pragma omp parallel for
    for(int i=0; i<LIMIT; i++)
        {
        array[i] = omp_get_thread_num();
        }

    for(int i=0; i<LIMIT; i++)
        {
        printf("%u ", array[i]);
        }
    printf("\n");

    }


