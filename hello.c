#include <common.h>
#include <exports.h>
#include <omp.h>

int hello(int argc, char *const argv[])
    {
    app_startup(argv);

    #pragma omp parallel num_threads(7)
    printf("hello, world! %d\n", omp_get_thread_num());

    return (0);
    }
