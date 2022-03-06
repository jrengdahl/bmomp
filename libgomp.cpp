#include <stdio.h>
#include <stdint.h>
#include <omp.h>
#include "libgomp.h"

OS_STACK<GOMP_STACK_SIZE> gomp_stacks[GOMP_NUM_THREADS] ALIGN(GOMP_STACK_SIZE);

OS_PORT workers[GOMP_NUM_THREADS];

typedef void (*FN)(void *);

struct job
    {
    FN fn;
    void *data;
    bool done;
    };

job jobs[GOMP_NUM_THREADS];


static uint32_t


extern "C"
int omp_get_num_threads()
    {
    return GOMP_NUM_THREADS;
    }



IGNORE_FLAGS("-Wvolatile-register-var")                         // ignore warnings about volatile registers
extern "C"
int omp_get_thread_num()
    {
    register volatile uintptr_t r9 __asm__("r9");

    return (r9>>GOMP_STACK_SHIFT) & ((1<<GOMP_TID_BITS)-1);
    }
END_IGNORE_FLAGS("-Wvolatile-register-var")



OS_THREAD(gomp_worker, tid)
    {
    yield();                                // shift the new thread to the correct core
    jobs[tid].done = false;

    while(true)
        {
        workers[tid].wait();

        FN fn = jobs[tid].fn;
        void *data = jobs[tid].data;
        (*fn)(data);
        jobs[tid].done = true;
        }
    }



extern "C"
void GOMP_parallel(
    void (*fn) (void *),                // the thread code
    void *data,                         // the local data
    unsigned num_threads,               // the requested number of threads
    unsigned int flags unused)          // flags (ignored for now)
    {
    if(num_threads > GOMP_NUM_THREADS)
        {
        num_threads = GOMP_NUM_THREADS;
        }


    for(int i=num_threads-1; i>=0; i--)
        {
        jobs[i].fn = fn;
        jobs[i].data = data;
        jobs[i].done = false;
        workers[i].wake();        
        }
    
    for(int i=0; i<num_threads; i++)
        {
        while(jobs[i].done == false
            {
            yield();
            }
        }
    }


void gomp_init()
    {
    char name[16];

    for(int i=0; i<num_threads; i++)
        {
        snprintf(name, sizeof(name), "gomp%d", i);
        gomp_stacks[i].spawn(name, gomp_worker, i);
        }
    }



