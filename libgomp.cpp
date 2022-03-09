#include <stdint.h>
#include <omp.h>
#include "thread.hpp"
#include "threadFIFO.hpp"
#include "libgomp.hpp"

char gomp_stacks[GOMP_NUM_THREADS][GOMP_STACK_SIZE] __attribute__((__aligned__((GOMP_STACK_SIZE))));

typedef void JOBFN(void *);

struct job
    {
    JOBFN *fn;
    void *data;
    bool done;
    Thread worker;
    };

job jobs[GOMP_NUM_THREADS];


extern "C"
int omp_get_num_threads()
    {
    return GOMP_NUM_THREADS;
    }


extern "C"
int omp_get_thread_num()
    {
    unsigned sp;

    __asm__ __volatile__("    mov %[sp], sp" : [sp]"=r"(sp));

    return (sp>>GOMP_STACK_SHIFT) & GOMP_TID_MASK;
    }



void gomp_worker()
    {
    int tid = omp_get_thread_num();

    jobs[tid].done = false;

    while(true)
        {
        jobs[tid].worker.suspend();

        JOBFN *fn = jobs[tid].fn;
        void *data = jobs[tid].data;
        (*fn)(data);
        jobs[tid].done = true;
        }
    }



extern "C"
void GOMP_parallel(
    JOBFN *fn,                                      // the thread code
    void *data,                                     // the local data
    unsigned num_threads,                           // the requested number of threads
    unsigned flags __attribute__((__unused__)))     // flags (ignored for now)
    {
    if(num_threads == 0 || num_threads > GOMP_NUM_THREADS)
        {
        num_threads = GOMP_NUM_THREADS;
        }


    for(int i=num_threads-1; i>=0; i--)
        {
        jobs[i].fn = fn;
        jobs[i].data = data;
        jobs[i].done = false;
        jobs[i].worker.resume();
        }
    
    for(unsigned i=0; i<num_threads; i++)
        {
        while(jobs[i].done == false)
            {
            yield();
            }
        }
    }


void libgomp_init()
    {
    for(int i=0; i<GOMP_NUM_THREADS; i++)
        {
        Thread::spawn(gomp_worker, gomp_stacks[i]);         
        }
    }



