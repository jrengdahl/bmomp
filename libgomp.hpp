#ifndef LIBGOMP_H
#define LIBGOMP_H

#define GOMP_STACK_SIZE 1024

#define GOMP_NUM_THREADS 32
#define GOMP_TID_MASK (GOMP_NUM_THREADS-1)

#define GOMP_NUM_TEAMS 16

extern void libgomp_init();

#endif // LIBGOMP_H
