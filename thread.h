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
    uint32_t r9;
    uint32_t sp;
    uint32_t lr;

    public:

    void resume();
    void suspend();
    static void start(THREADFN *fn, char *sp);

    template<unsigned N>
    static void spawn(THREADFN *fn,         // code
                      char (&stack)[N])     // reference to the stack)
        {
        start(fn, &stack[N-8]);
        }


    };




#endif // THREAD_H
