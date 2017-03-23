/*! exports.cpp */

#include <datatools/version.hpp>

#include "defs.h"

#include <cstring>

void testThatTest()
{
    testThat(strcmp(dt::version(), "0.1.0") == 0);
}

setupSuite(exports)
{
    addTest(testThatTest);
}
