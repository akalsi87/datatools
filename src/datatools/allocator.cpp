/*! allocator.cpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#include "datatools/allocator.hpp"

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <Windows.h>
#else
#include <mmap.h>
#endif

#include <cstdlib>
#include <new>

namespace dt
{

namespace
{

Address defMalloc(AllocState*, size_t sz)
{
    return new (std::nothrow) char[sz];
}

void defFree(AllocState*, void* ptr)
{
    delete[] static_cast<char*>(ptr);
}

static size_t const PAGE_SIZE = 4096;

Address defMmap(AllocState*, size_t sz)
{
    sz = (sz + (PAGE_SIZE - 1)) & (~(PAGE_SIZE - 1));
    sz += PAGE_SIZE;
#ifdef _WIN32
    Address mem = VirtualAlloc(0, sz, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
    Address mem = mmap(0, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    mem = (mem == ((Address)-1)) ? 0 : mem;
#endif
    auto szptr = static_cast<size_t*>(mem);
    *szptr = sz;
    return szptr + (PAGE_SIZE/sizeof(size_t));
}

void defMunmap(AllocState*, void* ptr)
{
    auto szptr = static_cast<size_t*>(ptr);
    szptr -= (PAGE_SIZE) / sizeof(size_t);
    size_t const sz = *szptr;
#ifdef _WIN32
    (void)VirtualFree(szptr, sz, MEM_RELEASE);
#else
    (void)munmap(szptr, sz);
#endif
}

} // namespace

Allocator const& defaultAllocator()
{
    static Allocator const Default(0, defMalloc, defFree, 0);
    return Default;
}

Allocator const& pageAllocator()
{
    static Allocator const Page(0, defMmap, defMunmap, 0);
    return Page;
}

namespace
{

static Address const MAGIC = reinterpret_cast<Address>(42);
static size_t const ALIGN = 8;

struct ArenaSegment
{
    size_t used;
    size_t size;
    ArenaSegment* next;

    Address getOffset(size_t const off) const
    {
        return ((char*)(this+1)) + off;
    }

    Address allocate(size_t sz)
    {
        size_t const alignedSize = (sz + (ALIGN-1)) & (~(ALIGN-1));
        sz = alignedSize;
        if (sz + used > size) return nullptr;
        Address mem = this->getOffset(used);
        used += sz;
        return mem;
    }
};

struct Arena
{
    Allocator const& baseAlloc;
    size_t defaultSize;
    ArenaSegment* top;
    Address _magic;

    Arena(Allocator const& base, size_t def)
      : baseAlloc(base), defaultSize(def), top(nullptr), _magic(MAGIC)
    {
        auto arenaSegment = reinterpret_cast<ArenaSegment*>(this+1);
        *arenaSegment = { 0, def, 0 };
        top = arenaSegment;
    }

    ArenaSegment* createNewSegment(size_t sz, ArenaSegment* next)
    {
        size_t const alignedSize = (sz + (ALIGN-1)) & (~(ALIGN-1));
        sz = alignedSize;
        sz += sizeof(ArenaSegment);
        if (sz < defaultSize) {
            sz = defaultSize;
        }
        Address mem = baseAlloc.allocate(sz);
        if (!mem) return nullptr;
        auto arenaSegment = static_cast<ArenaSegment*>(mem);
        *arenaSegment = { 0, sz, next };
        return arenaSegment;
    }

    static
    Address allocate(AllocState* state, size_t sz)
    {
        auto arena = static_cast<Arena*>(state);
        assert(arena);
        Address mem = arena->top->allocate(sz);
        if (!mem) {
            if (auto newTop = arena->createNewSegment(sz, arena->top)) {
                arena->top = newTop;
                mem = newTop->allocate(sz);
                assert(mem);
            }
        }
        return mem;
    }

    static
    void deallocate(AllocState* state, Address ptr)
    { }

    static
    bool teardown(AllocState* state)
    {
        auto arena = static_cast<Arena*>(state);
        auto const baseTop = reinterpret_cast<ArenaSegment*>(arena+1);
        auto top = arena->top;
        while (top != baseTop) {
            auto nextTop = top->next;
            arena->baseAlloc.deallocate(top);
            top = nextTop;
        }
        arena->baseAlloc.deallocate(state);
        return true;
    }
};



Arena* createArena(size_t defaultSize, Allocator const& baseAlloc)
{
    Address mem = baseAlloc.allocate(sizeof(Arena) + sizeof(ArenaSegment) + defaultSize);
    if (!mem) return nullptr;
    new (mem) Arena(baseAlloc, defaultSize);
    return static_cast<Arena*>(mem);
}

} // namespace

Allocator createArenaAllocator(size_t defaultSize, Allocator const& baseAlloc)
{
    Arena* arena = createArena(defaultSize, baseAlloc);
    if (!arena) {
        return Allocator(0, defMalloc, defFree, 0);
    } else {
        return Allocator(arena, Arena::allocate, Arena::deallocate, Arena::teardown);
    }
}

} // namespace dt
