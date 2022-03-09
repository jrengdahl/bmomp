// threadFIFO
//
// A threadFIFO is a subclass of FIFO. The data type stored in the FIFO are Threads.
// The inherited member functions are not much use.
// The subclass member functions, suspend and resume, enable multiple threads to suspend at
// a threadFIFO. When a threadFIFO is resumed the oldest suspended thread is resumed.
// One notable use of a threadFIFO is the DeferFIFO, which is used to support yield and
// timesharing among multiple threads.


#ifndef THREADFIFO_HPP
#define THREADFIFO_HPP

#include "thread.hpp"
#include "FIFO.hpp"

static const unsigned DEFER_FIFO_DEPTH = 16;

template<unsigned N>
class threadFIFO : public FIFO<Thread, N>
    {
    public:

    __attribute__((__noinline__))
    void suspend()
        {
        uint32_t curNextIn;                                                         // copy of nextIn
        uint32_t newNextIn;                                                         // updated index

        curNextIn = this->nextIn;                                                   // get the index of the slot next to be written
        newNextIn = curNextIn + 1;                                                  // calc index of next slot
        if(newNextIn > N)
            {
            newNextIn = 0;
            }
        if(newNextIn == this->nextOut)                                              // if pointers collide it's full, return false
            {
            return;                                                                 // FIFO is full, cannot suspend here, TODO this probably should be an assert
            }

        __asm__ __volatile__(

    "   mov ip, sp                                      \n"
    "   stmia %[th], {r4, r5, r6, r7, r8, r10, ip, lr}   \n"
    "   ldmia r11!,  {r4, r5, r6, r7, r8, r10, ip, lr}   \n"
    "   mov sp, ip                                      \n"
        :
        : [th]"r"(&(this->Data)[curNextIn])
        :
        );

        this->nextIn = newNextIn;                                                   // store new index
        }


    __attribute__((__noinline__))
    void resume()
        {
        uint32_t curNextOut;                                                        // copy of nextIn
        uint32_t newNextOut;                                                        // updated index

        curNextOut = this->nextOut;                                                 // get the current output index
        if(curNextOut == this->nextIn)                                              // if FIFO is empty
            {
            return;                                                                 // return, FIFO is empty, nothing to resume
            }

        __asm__ __volatile__(
    "   mov ip, sp                                      \n"
    "   stmdb r11!,  {r4, r5, r6, r7, r8, r10, ip, lr}  \n"
    "   ldmia %[th], {r4, r5, r6, r7, r8, r10, ip, lr}  \n"
    "   mov sp, ip                                      \n"
        :
        : [th]"r"(&(this->Data)[curNextOut])
        :
        );

        newNextOut = curNextOut + 1;                                                // calc index of next slot
        if(newNextOut > N)
            {
            newNextOut = 0;
            }
        this->nextOut = newNextOut;                                                 // save new index
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

