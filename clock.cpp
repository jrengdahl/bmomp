#include <stdio.h>
#include "exports.hpp"

extern "C"
unsigned long clock()
    {
    return get_timer(0) * 1000;
    }    
