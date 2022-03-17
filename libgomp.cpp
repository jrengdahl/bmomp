// libgomp.cpp
//
// A subset of libgomp for bare metal.
// This is a simplistic implementation for a single-core processor for learning, development, and testing.
//
// Target is an ARM CPU with very simple bare metal threading support.

#include <stdint.h>
#include <omp.h>
#include "thread.hpp"
#include "threadFIFO.hpp"
#include "libgomp.hpp"

// The worker thread's stacks
// The thread number can be determined by  looking at certain bits of the sp.
static char gomp_stacks[GOMP_NUM_THREADS][GOMP_STACK_SIZE] __attribute__((__aligned__((8))));

// a thread that waits for a job from OMP
typedef void JOBFN(void *);

static int team = 0;

// a job
struct job
    {
    Thread worker;          // the thread
    JOBFN *fn;              // the thread function generated by OMP
    void *data;             // the thread's local data pointer
    bool running = false;   // a flag which indicates that the thread is running
    int team = 0;           // the current team number
    bool arrived = false;
    bool mwaiting = false;
    };

// an array of jobs
static job jobs[GOMP_NUM_THREADS];

static bool mutex[GOMP_NUM_TEAMS] = {0};
static int mindex = 0;

// return the number of threads available
extern "C"
int omp_get_num_threads()
    {
    return GOMP_NUM_THREADS;
    }


extern "C"
int omp_get_thread_num()
    {
    uintptr_t sp;
    uintptr_t base = (uintptr_t)&gomp_stacks;

    __asm__ __volatile__("    mov %[sp], sp" : [sp]"=r"(sp));

    return ((sp-base)>>GOMP_STACK_SHIFT) & GOMP_TID_MASK;
    }



// this is the code for every member of the thread pool 

static void gomp_worker()
    {
    int id = omp_get_thread_num();

    while(true)
        {
        jobs[id].running = false;
        jobs[id].worker.suspend();                 //  wait for an assignnment
        jobs[id].running = true;

        JOBFN *fn = jobs[id].fn;                    // run the assigned code
        void *data = jobs[id].data;
        (*fn)(data);
        }
    }



extern "C"
void GOMP_parallel(
    JOBFN *fn,                                      // the thread code
    void *data,                                     // the local data
    unsigned num_threads,                           // the requested number of threads
    unsigned flags __attribute__((__unused__)))     // flags (ignored for now)
    {

    // TODO fix this to handle teams of less than max threads
    // starting at other than i=0

    if(num_threads == 0 || num_threads > GOMP_NUM_THREADS)
        {
        num_threads = GOMP_NUM_THREADS;
        }

    ++team;

    // start each member of the team, from highest to lowest
    for(int i=num_threads-1; i>=0; i--)
        {
        jobs[i].fn = fn;
        jobs[i].data = data;
        jobs[i].team = team;
        jobs[i].arrived = false;
        jobs[i].worker.resume();
        }
    
    for(unsigned i=0; i<num_threads; i++)
        {
        while(jobs[i].running == true)
            {
            yield();
            }
        }

    --team;
    }


extern "C"
void GOMP_barrier()
    {
    int id = omp_get_thread_num();
    int num = omp_get_num_threads();

    jobs[id].arrived = true;                    // signal that this thread has reached the barrier

    for(int i=0; i<num; i++)                    // see if any other threads in the team have not yet reached the barrier
        {
        if(jobs[i].running == true
        && jobs[i].team == team
        && jobs[i].arrived == false)
            {                                   // get here if any team member has not yet arrived
            jobs[id].worker.suspend();          // suspend this thread until all other threads have arrived
            return;                             // when resumed, some other thread has done all the barrier cleanup work, so just keep going
            }
        }

    // get here if all threads in the team have arrived at the barrier

    for(int i=0; i<num; i++)                    // clear the arrived flag for all team members
        {
        if(jobs[i].running == true
        && jobs[i].team == team)
            {
            jobs[i].arrived = false;            // clear the arrived-at-barrier flag
            }
        }

    for(int i=0; i<num; i++)                    // resume all other threads in the team
        {
        if(i != id                              // don't try to resume self
        && jobs[i].running == true
        && jobs[i].team == team)
            {
            jobs[i].worker.resume();            // resume the team member
            }
        }
    }


extern "C"
void GOMP_critical_start()
    {
    int id = omp_get_thread_num();

    while(mutex[team] == true)
        {
        jobs[id].mwaiting = true;
        jobs[id].worker.suspend();              // suspend this thread until it can grab the mutex
        jobs[id].mwaiting = false;
        }

    mutex[team] = true;
    }


extern "C"
void GOMP_critical_end()
    {
    int num = omp_get_num_threads();

    mutex[team] = false;

    for(int i=0; i<num; i++)                    // resume all other threads in the team
        {
        int m = mindex;
        mindex = (mindex+1)%num;

        if(jobs[m].running == true
        && jobs[m].team == team
        && jobs[m].mwaiting == true)
            {
            jobs[m].worker.resume();            // resume the next thread in the rotation
            return;
            }
        }
    }




// Powerup initialization of libgomp.
// Must be called after thread and threadFIFO are setup.
// It spawns a pool of OMP worker threads.

void libgomp_init()
    {
    for(int i=0; i<GOMP_NUM_THREADS; i++)
        {
        Thread::spawn(gomp_worker, gomp_stacks[i]);         
        }
    }



