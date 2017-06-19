#include "test_fcore.h"
#include <vector>

#include <iostream>

#ifndef NDEBUG
//    #include <vld.h>
#endif

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    for (int i = 0; i < 10; ++i)
        test_fcore();

    return 0;
}