#ifndef COMPILER_HPP
#define COMPILER_HPP

static inline void COMPILE_BARRIER()
    {
    __asm__ __volatile__ ("" :::  "memory");
    }

#endif // COMPILER_HPP
