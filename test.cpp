#include "thread.h"


extern "C"
void test0(place &x)
    {
    x.resume();
    }

extern "C"
void test1(place &x)
    {
    x.suspend();
    }
