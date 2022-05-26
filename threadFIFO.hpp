// threadFIFO
//
// A threadFIFO is a subclass of FIFO. The data type stored in the FIFO are Threads.
// The inherited member functions add and take are not much use.
// The subclass member functions, suspend and resume, enable multiple threads to suspend at
// a threadFIFO. When a threadFIFO is resumed the oldest suspended thread is resumed.
// One notable use of a threadFIFO is the DeferFIFO, which is used to support yield and
// timesharing among multiple threads.


#ifndef THREADFIFO_HPP
#define THREADFIFO_HPP

#include "Compiler.hpp"
#include "thread.hpp"
#include "FIFO.hpp"

static const unsigned DEFER_FIFO_DEPTH = 63;

template<unsigned N>
class threadFIFO : public FIFO<Thread, N>
    {
    public:

    __attribute__((__optimize__("Os")))
    __attribute__((__noinline__))
    void suspend()
        {
        uint32_t curNextIn;                                                         // copy of nextIn
        uint32_t newNextIn;                                                         // updated index

        curNextIn = this->nextIn;                                                   // get the index of the slot next to be written
        newNextIn = (curNextIn + 1) % (N + 1);                                      // calc index of next slot
        if(newNextIn == this->nextOut)                                              // if pointers collide it's full, return false
            {
            return;                                                                 // FIFO is full, cannot suspend here, TODO this probably should be an assert
            }

        __asm__ __volatile__(

    "   mov ip, sp                          \n"
    "   stmia %[th], {r4-r8, r10-ip, lr}    \n"
    "   ldmia r9!,   {r4-r8, r10-ip, lr}    \n"
    "   mov sp, ip                          \n"
        :
        : [th]"r"(&(this->Data)[curNextIn])
        :
        );

        COMPILE_BARRIER;
        this->nextIn = newNextIn;                                                   // store new index
        }


    __attribute__((__optimize__("Os")))
    __attribute__((__noinline__))
    void resume()
        {
        uint32_t curNextOut;                                                        // copy of nextIn

        curNextOut = this->nextOut;                                                 // get the current output index
        if(curNextOut == this->nextIn)                                              // if FIFO is empty
            {
            return;                                                                 // return, FIFO is empty, nothing to resume
            }

        __asm__ __volatile__(
    "   mov ip, sp                          \n"
    "   stmdb r9!,   {r4-r8, r10-ip, lr}    \n"
    "   ldmia %[th], {r4-r8, r10-ip, lr}    \n"
    "   mov sp, ip                          \n"
        :
        : [th]"r"(&(this->Data)[curNextOut])
        :
        );

        COMPILE_BARRIER;
        this->nextOut = (curNextOut + 1) % (N + 1);                                 // calc index of next slot
        }
    };


extern threadFIFO<DEFER_FIFO_DEPTH> DeferFIFO;


// suspend the current thread at the DeferFIFO.
// It will be resumed later by the background thread.

static inline void yield()
    {
    DeferFIFO.suspend();
    }


// Called by the backgroud thread to resume any supended threads in the DeferFIFO.

static inline void undefer()
    {
    DeferFIFO.resume();
    }


#endif // THREADFIFO_HPP

