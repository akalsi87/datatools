/*! allocator.cpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#include "datatools/allocator.hpp"

#include <cstdlib>
#include <new>

namespace dt
{

Address defMalloc(AllocState*, size_t sz)
{
    return new (std::nothrow) char[sz];
}

void defFree(AllocState*, void* ptr)
{
    delete[] static_cast<char*>(ptr);
}

Allocator const& defaultAllocator()
{
    static Allocator const Default(0, defMalloc, defFree, 0);
    return Default;
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
