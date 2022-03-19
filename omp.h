#ifndef _LIBGOMP_OMP_LOCK_DEFINED
#define _LIBGOMP_OMP_LOCK_DEFINED

typedef int omp_lock_t;

struct omp_nest_lock_t
    {
    int lock;
    int count;
    int owner;
    };

#endif // _LIBGOMP_OMP_LOCK_DEFINED

#include_next <omp.h>
