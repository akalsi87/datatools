/*! exports.cpp */

#include <datatools/version.hpp>
#include <datatools/allocator.hpp>
#include <datatools/array.hpp>
#include <datatools/mapfile.hpp>

#include "defs.h"

#include <cstring>

namespace
{

int returnOne()
{
    return 1;
}

} // namespace

void funcPtrType()
{
    testThat(dt::FuncPtr<int(void)>(returnOne)() == 1);
}

void version()
{
    // also update ChangeLog.txt when this fails
    testThat(std::strcmp(dt::version(), "0.1.0") == 0);
}

void allocatorDefault()
{
    auto const& alloc = dt::defaultAllocator();
    dt::Address mem = alloc.allocate(20);
    testThat(mem);
    alloc.deallocate(mem);
}

void allocatorPage()
{
    auto const& alloc = dt::pageAllocator();
    auto mem = (char*)alloc.allocate(2);
    testThat(mem != 0);
    // scribble at mem end
    mem[4095] = 'a';
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

void array()
{
    {/* empty array */
        dt::Array<int> arr;
        testThat(arr.size() == 0);
        arr.reset(10);
        testThat(arr.size() == 10);
        for (size_t i = 0; i < 10; ++i) {
            testThat(arr[i] == 0);
        }
        arr.reset();
        testThat(arr.size() == 0);
    }
}

void mapFile()
{
    {// create and destroy with open
        dt::MappedFile f;
        testThat(f.open(__FILE__));
        auto data = f.getData();
        testThat(data != 0);
        testThat(data[0] == '/');
        testThat(data[1] == '*');
        testThat(data[2] == '!');
    }
    {// create and close
        dt::MappedFile f;
        testThat(f.open(__FILE__));
        f.close();
    }
}

setupSuite(exports)
{
    addTest(version);
    addTest(funcPtrType);
    addTest(allocatorDefault);
    addTest(allocatorPage);
    addTest(arenaAllocator);
    addTest(array);
    addTest(mapFile);
}
