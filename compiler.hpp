#ifndef COMPILER_HPP
#define COMPILER_HPP

#define COMPILE_BARRIER __asm__ __volatile__ ("" :::  "memory")
#define ALIGN(x) __attribute__ ((aligned ((x))))
#define SECTION(x) __attribute__ ((section (x)))
#define UNUSED __attribute__ ((unused))

#ifdef __SIZEOF_INT128__
typedef __uint128_t uint128_t;
typedef __int128_t int128_t;
#endif

#define CONCAT_(x, y) x##y
#define CONCAT(x,y) CONCAT_ (x,y)

template<typename T>
T max(T a, T b)
    {
    if(a>b)return a;
    else return b;
    }

template<typename T>
T min(T a, T b)
    {
    if(a<b)return a;
    else return b;
    }

#define OPTIMIZE(n) __attribute__((__optimize__(n)))
#define INLINE inline __attribute__((__always_inline__))
#define likely(x)   (__builtin_expect(((x) ? 1L : 0L), 1L) != 0L)
#define unlikely(x) (__builtin_expect(((x) ? 1L : 0L), 0L) != 0L)

#define IGNORE_WARNING(s)                       \
    _Pragma("GCC diagnostic push")              \
    _Pragma(__STR2__(GCC diagnostic ignored s))

#define END_IGNORE_WARNING(s)                   \
    _Pragma("GCC diagnostic pop")


#endif // COMPILER_HPP
