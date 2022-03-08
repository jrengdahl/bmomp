#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

typedef void THREADFN();

class Thread
    {
    private:

    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r10;
    uint32_t sp;
    uint32_t lr;

    // r9 is reserved by u-boot for the gd pointer
    // r11 is reserved by Thread for the thread stack pointer

    static void start(THREADFN *fn, char *sp);

    public:

    void resume();
    void suspend();

    template<unsigned N>
    static void spawn(THREADFN *fn,         // code
                      char (&stack)[N])     // reference to the stack)
        {
        start(fn, &stack[N-8]);
        }


    static void init();

    template<unsigned N>
    static bool done(char (&stack)[N])
        {
        return stack[N-4] == 1;
        }

    };




#endif // THREAD_H
