#include <stdint.h>

#include "thread.hpp"

const unsigned THREAD_DEPTH = 32;

static Thread thread_stack[THREAD_DEPTH];


void Thread::resume()
    {
    __asm__ __volatile__(

"   mov ip, sp                                      \n"
"   stmdb r11!, {r4, r5, r6, r7, r8, r10, ip, lr}   \n"
"   ldmia r0,   {r4, r5, r6, r7, r8, r10, ip, lr}   \n"
"   mov sp, ip                                      \n"
    );
    }

void Thread::suspend()
    {
    __asm__ __volatile__(

"   mov ip, sp                                      \n"
"   stmia r0,   {r4, r5, r6, r7, r8, r10, ip, lr}   \n"
"   ldmia r11!, {r4, r5, r6, r7, r8, r10, ip, lr}   \n"
"   mov sp, ip                                      \n"
    );
    }


void Thread::start(THREADFN *fn, char *newsp)
    {
    __asm__ __volatile__(

"   mov ip, sp                                      \n"
"   stmdb r11!, {r4, r5, r6, r7, r8, r10, ip, lr}   \n"

"   mov sp, %[newsp]                                \n"
"   mov r1, #0                                      \n"
"   str r1, [sp, #0]                                \n"
"   str r1, [sp, #4]                                \n"

"   blx %[fn]                                       \n"

"   mov r1, #1                                      \n"
"   str r0, [sp, #0]                                \n"
"   str r1, [sp, #4]                                \n"

"   ldmia r11!, {r4, r5, r6, r7, r8, r10, ip, lr}   \n"
"   mov sp, ip                                      \n"

    :
    : [fn]"r"(fn), [newsp]"r"(newsp)
    : 
    );
    }


void Thread::init()
    {
    __asm__ __volatile__(
"   mov r11, %[tsp]"
    :
    : [tsp]"r"(&thread_stack[THREAD_DEPTH])
    :
    );
    }
