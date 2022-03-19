# bmomp
Bare Metal OpenMP

At the moment this is a very rough development project, with a lot of experimentation going on.

The objective is to get OpenMP working on a bare metal microprocessor. The initial experimentation
is being done on a Cortex-M7. OpenMP may not make a lot of sense on a single-core CPU, but with an
underlying thread switching mechanism, it is a useful platform for initial development. Later efforts
will use a NXP quad-core Cortex-A9.

The base software is u-boot running from flash (see https://github.com/jrengdahl/u-boot).

hello.c is a simple "hello, world!" that can be downloaded to u-boot and runs using the u-boot API.

hello_cpp.cpp is a C++ version of hello.

The files thread.hpp and thread.cpp provide a simple but very fast thread switching mechanism.
It permanently allocates R11 as the thread stack pointer. Note that r9 is already reserved for the u-boot
global data pointer. The use of R11 for the thread stack pointer has two reasons:
- it makes thread switching very simple and fast, and
- it reduces the number of registers required to implement the thread switch, which enables it to be coded
  using only the non-volatile registers which are not part of the saved context.

libgomp.cpp implements:
- #pragma omp parallel
- #pragma omp parallel for
- #pragma omp parallel for reduction
- #pragma omp barrier
- #pragma omp critical
- #pragma omp atomic
- #pragma omp master
- #pragma omp single
- #pragma omp sections
- omp_lock_t and its operators
- omp_get_num_threads
- omp_get_thread_num

Currently these implementations are primitive and only work on a single-core processor, almost like pseudocode
for a multi-core version to be developed later. For example, the mutex underlying "critical" is not really
atomic. This implementation does give correct results on a single-core processor with the above-mentioned
non-preemptive threading system.

I've been going through Tim Mattson's OpenMP tutorial (https://www.openmp.org/resources/tutorials-articles).
As I study each example I implement the support for it in libgomp.cpp and create a test (omp\<n\>.cpp).

And no, I don't know what I'm doing. Like I said, I'm learning OpenMP from the tutorial as I develop the
implementaion. This actually seems like a good way to learn it thoroughly.
