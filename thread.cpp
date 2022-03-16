// Thread.cpp
// Implementation of the Thread class.

#include <stdint.h>
#include "thread.hpp"

// Define the maximum depth of teh thread stack -- how deep can "resume" calls be nested.
// I believe 32 is a ridiculously large nesting depth.
const unsigned THREAD_DEPTH = 32;

// This is the pending thread stack.
static Thread thread_stack[THREAD_DEPTH];

// Push the current thread onto the pending stack, and
// resume a thread saved in a Thread object.
void Thread::resume()
    {
    __asm__ __volatile__(
"   mov ip, sp                                      \n"         // sp cannot be pushed, so move it to ip first
"   stmdb r11!, {r4, r5, r6, r7, r8, r10, ip, lr}   \n"         // push all the non-volatile registers of the current thread onto the thread stack
"   ldmia r0,   {r4, r5, r6, r7, r8, r10, ip, lr}   \n"         // load all the non-volatile registers of the new thread into the registers
"   mov sp, ip                                      \n"         // move ip back to sp
    );                                                          // return to the new thread
    }

// Suspend the current thread into a Thread object,
// pop the next thread from the pending stack into the
// register set, and resume running it.
void Thread::suspend()
    {
    __asm__ __volatile__(
"   mov ip, sp                                      \n"         // save sp in ip so it can be saved using STM
"   stmia r0,   {r4, r5, r6, r7, r8, r10, ip, lr}   \n"         // save the non-volatile registers of the current thread into the Thread instance
"   ldmia r11!, {r4, r5, r6, r7, r8, r10, ip, lr}   \n"         // pop the non-volatile registers of the most recently active thread on the pending stack
"   mov sp, ip                                      \n"         // restore sp
    );                                                          // return to the previously pending thread
    }


// Start a new thread, given it's code and initial stack pointer.
// args: fn:    a pointer to a subroutine which will be run as a thread
//       newsp: the initial stack pointer of the new thread

// The newsp must point to the second word from the high end of the stack.
// The word pointed to by newsp is reserved to pass the thread's return value
// during the rendezvous at the completion of the thread. The second word
// past newsp is used for the thread's "done" flag.
//
// The thread which calls spawn is pushed to the pending stack as if it
// had called "resume". The calling thread will be resumed the first time
// the new thread suspends itself. The calling thread will continue
// executing after the call to "start". It will most likely continue executing
// before the new thread terminates.
//
// When the new thread terminates by returning from it's subroutine, it returns
// to this code, which will run the next thread from the pending stack. This
// is not necessarily the thread which created the terminated thread.

void Thread::start(THREADFN *fn, char *newsp)
    {
    __asm__ __volatile__(
"   mov ip, sp                                      \n"             // save the sp of the calling thread in a register that can be pushed
"   stmdb r11!, {r4, r5, r6, r7, r8, r10, ip, lr}   \n"             // push the calling thread onto the pending thread stack

"   mov sp, %[newsp]                                \n"             // setup the new thread's stack
"   mov r1, #0                                      \n"             //
"   str r1, [sp, #0]                                \n"             // clear the return value
"   str r1, [sp, #4]                                \n"             // and the "done" flag

"   blx %[fn]                                       \n"             // start executing the code of the new thread

// when the new thread terminates, it returns here...

"   mov r1, #1                                      \n"             //
"   str r0, [sp, #0]                                \n"             // save the return value
"   str r1, [sp, #4]                                \n"             // set the "done" flag

"   ldmia r11!, {r4, r5, r6, r7, r8, r10, ip, lr}   \n"             // pop the next thread from the pending thread stack, and return to it
"   mov sp, ip                                      \n"

    :
    : [fn]"r"(fn), [newsp]"r"(newsp)
    : 
    );
    }


// init the threading system
// At present, this consists only of setting the pending thread stack pointer
void Thread::init()
    {
    __asm__ __volatile__(
"   mov r11, %[tsp]"                                                // init the thread stack pointer
    :
    : [tsp]"r"(&thread_stack[THREAD_DEPTH])
    :
    );
    }
