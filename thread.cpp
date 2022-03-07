#include <stdint.h>

#include "thread.h"

const unsigned THREAD_DEPTH = 32;

static Thread thread_stack[THREAD_DEPTH];

static Thread *thread_stack_pointer __attribute__((__unused__)) = &thread_stack[THREAD_DEPTH];


void Thread::resume()
    {
    __asm__ __volatile__(

"   ldr r3, =thread_stack_pointer                   \n"
"   ldr r2, [r3]                                    \n"
"   mov ip, sp                                      \n"
"   stmdb r2!, {r4, r5, r6, r7, r8, r9, ip, lr}     \n"
"   str r2, [r3]                                    \n"

"   ldmia r0, {r4, r5, r6, r7, r8, r9, ip, lr}      \n"
"   mov sp, ip                                      \n"
    :
    :
    : 
    );
    }

void Thread::suspend()
    {
    __asm__ __volatile__(

"   mov ip, sp                                      \n"
"   stmia r0, {r4, r5, r6, r7, r8, r9, ip, lr}      \n"

"   ldr r3, =thread_stack_pointer                   \n"
"   ldr r2, [r3]                                    \n"
"   ldmia r2!, {r4, r5, r6, r7, r8, r9, ip, lr}     \n"
"   mov sp, ip                                      \n"
"   str r2, [r3]                                    \n"
    :
    :
    :
    );
    }


void Thread::start(THREADFN *fn, char *newsp)
    {
    __asm__ __volatile__(

"   ldr r3, =thread_stack_pointer                   \n"
"   ldr r2, [r3]                                    \n"
"   mov ip, sp                                      \n"
"   stmdb r2!, {r4, r5, r6, r7, r8, r9, ip, lr}     \n"
"   str r2, [r3]                                    \n"

"   mov sp, %[newsp]                                \n"
"   mov r1, #0                                      \n"
"   str r1, [sp, #0]                                \n"
"   str r1, [sp, #4]                                \n"

"   blx %[fn]                                       \n"

"   mov r1, #1                                      \n"
"   str r0, [sp, #0]                                \n"
"   str r1, [sp, #4]                                \n"

"   ldr r3, =thread_stack_pointer                   \n"
"   ldr r2, [r3]                                    \n"
"   ldmia r2!, {r4, r5, r6, r7, r8, r9, ip, lr}     \n"
"   mov sp, ip                                      \n"
"   str r2, [r3]                                    \n"

    :
    : [fn]"r"(fn), [newsp]"r"(newsp)
    : 
    );
    }


