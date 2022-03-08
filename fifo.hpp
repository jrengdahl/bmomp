//////////////////////////////////////////////////////////////////////////////
// FIFO.hpp
// A single-writer-single-reader lock-free/wait-free FIFO.
// Multi-thread and multi-core.
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef FIFO_HPP
#define FIFO_HPP


///////////////////////////////////////////////////////////////////////////////
/// @class Fifo
///
/// @param T      - Type of data this FIFO will hold.
/// @param N      - Number of entries this FIFO will hold.
///
/////////////////////////////////////////////////////////////////////////////
template<typename T, unsigned N>
class FIFO
    {
    private:

    unsigned nextIn = 0;        // Index of next entry to be written, align to double-word boundary so ldp can be used by optimizer
    unsigned nextOut = 0;       // Index of next entry to be read.

    T Data[N+1];                // Data store (one more than capacity, since m_nextIn==m_nextOut is empty, but m_nextIn+1==m_nextOut is full).


    public:

    ///////////////////////////////////////////////////////////////////////////////
    //  FIFO::operator bool
    ///
    /// @details returns true if FIFO has any contents
    ///////////////////////////////////////////////////////////////////////////////

    inline operator bool() { return nextIn != nextOut; }


    ///////////////////////////////////////////////////////////////////////////////
    //  FIFO::add
    ///
    /// @details Adds an entry to the FIFO.
    ///
    /// @param value  The value to be added. <b>Must not be nil!</b>
    ///
    /// @return bool
    ///     @retval true   The value was added to the FIFO.
    ///     @retval false  The FIFO is full, the value was not added.
    ///////////////////////////////////////////////////////////////////////////////

    bool add(T value)
        {
        unsigned cNextIn;                                                           // copy of nextIn
        unsigned newNextIn;                                                         // updated index

        cNextIn = nextIn;                                                           // get the index of the slot next to be written
        newNextIn = cNextIn + 1;                                                    // calc index of next slot
        if(newNextIn > N)
            {
            newNextIn = 0;
            }
        if(newNextIn == nextOut)                                                    // if pointers collide it's full, return false
            {
            return false;
            }

        Data[cNextIn] = value;                                                      // store the data to the owned slot

        nextIn = newNextIn;                                                         // store new index

        return true;                                                                // return success
        }


    /////////////////////////////////////////////////////////////////////////////
    //  FIFO::Take
    ///
    /// @details Takes an entry from the FIFO.
    ///
    /// arg: reference to where to put the value taken
    /// return: true if a value was taken, false if the FIFO was empty
    ////////////////////////////////////////////////////////////////////////////

    bool take(T &value)
        {
        unsigned cNextOut;                                                          // copy of nextIn
        unsigned newNextOut;                                                        // updated index

        cNextOut = nextOut;                                                         // get the current output index
        if(cNextOut == nextIn)                                                      // if FIFO is empty
            {
            return false;                                                           // return false, the FIFO was empty and no value was returned
            }

        value = Data[cNextOut];                                                     // get the value from the current slot

        newNextOut = cNextOut + 1;                                                  // calc index of next slot
        if(newNextOut > N)
            {
            newNextOut = 0;
            }
        nextOut = newNextOut;                                                       // save new index

        return true;                                                                // return true, a value was returned
        }
    };


#endif // FIFO_HPP

