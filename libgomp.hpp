#ifndef LIBGOMP_H
#define LIBGOMP_H

#define GOMP_STACK_SIZE 1024

#define GOMP_NUM_THREADS 40
#define GOMP_DEFAULT_NUM_THREADS 8

#define GOMP_NUM_TEAMS 16

#define GOMP_NUM_TASKS (GOMP_NUM_THREADS * 2)

extern void libgomp_init();

#endif // LIBGOMP_H
