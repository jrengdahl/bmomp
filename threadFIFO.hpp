#ifndef THREADFIFO_HPP
#define THREADFIFO_HPP

#include "thread.hpp"
#include "FIFO.hpp"

template<unsigned N>
class threadFIFO : public FIFO<Thread, N>
    {
    public:

    __attribute__((__noinline__))
    void suspend()
        {
        uint32_t cNextIn;                                                           // copy of nextIn
        uint32_t newNextIn;                                                         // updated index

        cNextIn = this->nextIn;                                                     // get the index of the slot next to be written
        newNextIn = cNextIn + 1;                                                    // calc index of next slot
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
    "   stmia %[th], {r4, r5, r6, r7, r8, r9, ip, lr}   \n"
    "   ldmia r11!,  {r4, r5, r6, r7, r8, r9, ip, lr}   \n"
    "   mov sp, ip                                      \n"
        :
        : [th]"r"(&(this->Data)[cNextIn])
        :
        );

        this->nextIn = newNextIn;                                                   // store new index
        }


    __attribute__((__noinline__))
    void resume()
        {
        uint32_t cNextOut;                                                          // copy of nextIn
        uint32_t newNextOut;                                                        // updated index

        cNextOut = this->nextOut;                                                   // get the current output index
        if(cNextOut == this->nextIn)                                                // if FIFO is empty
            {
            return;                                                                 // return, FIFO is empty, nothing to resume
            }

        __asm__ __volatile__(
    "   mov ip, sp                                      \n"
    "   stmdb r11!,  {r4, r5, r6, r7, r8, r10, ip, lr}  \n"
    "   ldmia %[th], {r4, r5, r6, r7, r8, r10, ip, lr}  \n"
    "   mov sp, ip                                      \n"
        :
        : [th]"r"(&(this->Data)[cNextOut])
        :
        );

        newNextOut = cNextOut + 1;                                                  // calc index of next slot
        if(newNextOut > N)
            {
            newNextOut = 0;
            }
        this->nextOut = newNextOut;                                                 // save new index
        }
    };


#endif // THREADFIFO_HPP

