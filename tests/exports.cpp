/*! exports.cpp */

#include <datatools/version.hpp>
#include <datatools/allocator.hpp>

#include "defs.h"

#include <cstring>

int returnOne()
{
    return 1;
}

void funcPtrType()
{
    testThat(dt::FuncPtr<int(void)>(returnOne)() == 1);
}

void version()
{
    testThat(strcmp(dt::version(), "0.1.0") == 0);
}

void allocatorDefault()
{
    auto const& alloc = dt::defaultAllocator();
    dt::Address mem = alloc.allocate(20);
    testThat(mem);
    alloc.deallocate(mem);
}

void arenaAllocator()
{
    auto const& arena = dt::createArenaAllocator();
    char *mem0, *mem1, *mem2;
    testThat((mem0 = (char*)arena.allocate(1)) != 0);
    testThat((mem1 = (char*)arena.allocate(7)) != 0);
    testThat(mem0 != mem1);
    testThat((mem1-mem0) == 8);
    testThat((mem2 = (char*)arena.allocate(100000)) != 0);
    // scribble at end of mem
    mem2[99999] = 'a';
    testThat(mem2 != (mem1+8));
}

setupSuite(exports)
{
    addTest(version);
    addTest(funcPtrType);
    addTest(allocatorDefault);
    addTest(arenaAllocator);
}
