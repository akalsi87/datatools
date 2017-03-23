/*! exports.cpp */

#include <datatools/version.hpp>
#include <datatools/allocator.hpp>

#include "defs.h"

#include <cstring>

int returnOne()
{
    return 1;
}

void version()
{
    testThat(strcmp(dt::version(), "0.1.0") == 0);
    testThat(dt::FuncPtr<int(void)>(returnOne)() == 1);
}

setupSuite(exports)
{
    addTest(version);
}
