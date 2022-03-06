#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

class thread;

template<unsigned size>
uint32_t stack[size];

const unsigned THREAD_DEPTH = 32;
extern thread *thread_stack[THREAD_DEPTH];
extern unsigned thread_stack_pointer;

class thread
    {
    private:

    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t sp;
    uint32_t lr;

    char *stack;
    unsigned stack_size;

    public:

    void inline load()
        {
        __asm__ __volatile__(
            "ldm %[th],{r4-r9,ip,lr}    \n\t"
            "mov sp,ip                      "
        :
        : [th] "r"(this)
        : );
        }

    void inline save()
        {
        __asm__ __volatile__(
            "mov ip,sp                  \n\t"
            "stm %[th],{r4-r9,ip,lr}        "
        :
        : [th] "r"(this)
        : );
        }


    };



class place
    {
    private:

    thread *th = 0;

    public:

    void __attribute__((__noinline__)) resume()
        {
        thread *pth = th;

        if(pth == 0)return;
        th = 0;

        thread_stack[thread_stack_pointer]->save();
        --thread_stack_pointer;
        thread_stack[thread_stack_pointer] = pth;

        pth->load();
        }

    void __attribute__((__noinline__)) suspend()
        {
        thread *pth = thread_stack[thread_stack_pointer];

        pth->save();
        th = pth;

        ++thread_stack_pointer;
        thread_stack[thread_stack_pointer]->load();
        }


    };




#endif // THREAD_H
