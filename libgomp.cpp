// libgomp.cpp
//
// A subset of libgomp for bare metal.
// This is a simplistic implementation for a single-core processor for learning, development, and testing.
//
// Target is an ARM CPU with very simple bare metal threading support.
//
// These implementations only work on a single-core,non-preemtive system.


#include <stdint.h>
#include <omp.h>
#include "thread.hpp"
#include "threadFIFO.hpp"
#include "libgomp.hpp"

// The worker thread's stacks
// The thread number can be determined by  looking at certain bits of the sp.
static char gomp_stacks[GOMP_NUM_THREADS][GOMP_STACK_SIZE] __attribute__((__aligned__((4096))));    // TODO align to 4K for debug, later make this GOMP_STACK_SIZE

// a thread that waits for a job from OMP
typedef void JOBFN(void *);

static int team = 0;

// a job
// TODO -- perhaps rename this "worker"
struct job
    {
    Thread worker;          // the thread
    JOBFN *fn;              // the thread function generated by OMP
    void *data;             // the thread's local data pointer
    int team = 0;           // the current team number, nonzero if running
    unsigned single = 0;    // counts the number of "#pragma omp single" seen
    bool arrived = false;   // arrived at a barrier, waiting for other threads to arrive
    bool mwaiting = false;  // waiting on a mutex
    };

// an array of jobs
static job jobs[GOMP_NUM_THREADS];

// These arrays indexed by "team" should be converted to a pool of "team" structs.
// I don't think teams are necessarily nested in scope.
static bool mutex[GOMP_NUM_TEAMS] = {0};
static int mindex = 0;
static unsigned single[GOMP_NUM_TEAMS] = {0};
static int sections_count[GOMP_NUM_TEAMS] = {0};
static int sections[GOMP_NUM_TEAMS] = {0};
static int section[GOMP_NUM_TEAMS] = {0};



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

    return (sp-base)/GOMP_STACK_SIZE;
    }



// this is the code for every member of the thread pool 

static void gomp_worker()
    {
    int id = omp_get_thread_num();

    while(true)
        {
        jobs[id].team = 0;                          // zero the team to indicate the member has finished
        jobs[id].worker.suspend();                  //  wait for an new assignnment

        JOBFN *fn = jobs[id].fn;                    // run the assigned code
        void *data = jobs[id].data;
        (*fn)(data);
        }
    }



extern "C"
void GOMP_parallel(
    JOBFN *fn,                                      // the thread code
    void *data,                                     // the team local data
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

    mutex[team] = false;
    single[team] = 0;
    sections_count[team] = 0;
    sections[team] = 0;
    section[team] = 0;

    // setup each member of the team
    for(unsigned i=0; i<num_threads; i++)
        {
        jobs[i].fn = fn;
        jobs[i].data = data;
        jobs[i].arrived = false;
        jobs[i].mwaiting = false;
        jobs[i].single = 0;
        jobs[i].team = team;
        }

    // start each member of the team
    for(unsigned i=0; i<num_threads; i++)
        {
        jobs[i].worker.resume();
        }
    
    // wait for each team member to complete
    for(unsigned i=0; i<num_threads; i++)
        {
        while(jobs[i].team == team)
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
        if(jobs[i].team == team
        && jobs[i].arrived == false)
            {                                   // get here if any team member has not yet arrived
            jobs[id].worker.suspend();          // suspend this thread until all other threads have arrived
            return;                             // when resumed, some other thread has done all the barrier cleanup work, so just keep going
            }
        }

    // get here if all threads in the team have arrived at the barrier

    for(int i=0; i<num; i++)                    // clear the arrived flag for all team members
        {
        if(jobs[i].team == team)
            {
            jobs[i].arrived = false;            // clear the arrived-at-barrier flag
            }
        }

    for(int i=0; i<num; i++)                    // resume all other threads in the team
        {
        if(i != id                              // don't try to resume self
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
void GOMP_atomic_start()
    {
    GOMP_critical_start();
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

        if(jobs[m].team == team
        && jobs[m].mwaiting == true)
            {
            jobs[m].worker.resume();            // resume the next thread in the rotation
            return;
            }
        }
    }

extern "C"
void GOMP_atomic_end()
    {
    GOMP_critical_end();
    }



extern "C"
bool GOMP_single_start()
    {
    int id = omp_get_thread_num();

    if(jobs[id].single++ == single[team])
        {
        single[team]++;
        return true;
        }
    else
        {
        return false;
        }
    }





extern "C"
int GOMP_sections_next()                // for each thread that iterates the "sections"
    {
    if(section[team] > sections[team])  // if all sections have been executed
        {
        section[team] = 0;              // clear the index, it latches at zero
        }

    if(section[team] == 0)              // if all sections have been run
        {
        return 0;                       // return 0, select the "end" action and stop iterating
        }

    return section[team]++;             // otherwise return the current index and increment it
    }

extern "C"
int GOMP_sections_start(int num)        // each team member calls this once at the beginning of sections
    {
    if(sections_count[team] == 0)       // when the first thread gets here
        {
        sections[team] = num;           // capture the number of sections
        section[team] = 1;              // init to the first section
        }

    ++sections_count[team];             // count the number of threads that have started the sections

    return GOMP_sections_next();        // for the rest, start is the same as next
    }

extern "C"
void GOMP_sections_end()                // each thread runs this once when all the sections hae been executed
    {
    int num = omp_get_num_threads();

    if(sections_count[team] == num)     // if all team members have encountered the "start"
        {
        sections_count[team] = 0;       // re-arm the sections start, though note that some may still be in a section
        }

    GOMP_barrier();                     // hold everyone here until all have arrived
    }


/////////////
// LOCKING //
/////////////

extern "C"
void omp_init_lock(omp_lock_t *lock)
    {
    *lock = 0;
    }

extern "C"
void omp_set_lock(omp_lock_t *lock)
    {
    while(*lock)
        {
        yield();
        }

    *lock = 1;
    }

extern "C"
void omp_unset_lock(omp_lock_t *lock)
    {
    *lock = 0;
    }

extern "C"
int omp_test_lock(omp_lock_t *lock)
    {
    if(*lock)
        {
        return 0;
        }

    *lock = 1;
    return 1;
    }

extern "C"
void omp_destroy_lock(omp_lock_t *lock)
    {
    *lock = 0;
    }



////////////////////
// NESTED LOCKING //
////////////////////

extern "C"
void omp_init_nest_lock(omp_nest_lock_t *lock)
    {
    lock->lock = 0;
    lock->count = 0;
    lock->owner = 0;
    }

extern "C"
void omp_set_nest_lock(omp_nest_lock_t *lock)
    {
    int id = omp_get_thread_num();

    if(lock->lock && lock->owner == id)
        {
        ++lock->count;
        return;
        }
 
    while(lock->lock==1)
        {
        yield();
        }

    lock->lock = 1;
    lock->count = 1;
    lock->owner = id;
    }

extern "C"
void omp_unset_nest_lock(omp_nest_lock_t *lock)
    {
    // It is assumed that the caller matches the owner. This is not checked.
    // It is assumed that count>0. This is not checked.

    --lock->count;
    if(lock->count == 0)
        {
        lock->owner = 0;
        lock->lock = 0;
        }
    }

extern "C"
int omp_test_nest_lock(omp_nest_lock_t *lock)
    {
    int id = omp_get_thread_num();

    if(lock->lock && lock->owner == id)
        {
        ++lock->count;
        return lock->count;
        }
 
    if(lock->lock == 0)
        {
        lock->lock = 1;
        lock->count = 1;
        lock->owner = id;
        return 1;
        }

    return 0;
    }

extern "C"
void omp_destroy_nest_lock(omp_nest_lock_t *lock)
    {
    lock->lock = 0;
    lock->count = 0;
    lock->owner = 0;
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



