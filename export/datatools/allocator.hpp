/*! allocator.hpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#ifndef _DATATOOLS_ALLOCATOR_HPP_
#define _DATATOOLS_ALLOCATOR_HPP_

#include "version.hpp"

#include <utility>
#include <cassert>

namespace dt
{

template <class>
class FuncPtr;

template <class Return, class... ArgType>
class FuncPtr<Return(ArgType...)>
{
  private:
    typedef Return(*CFuncType)(ArgType...);
    CFuncType m_func;
  public:
    FuncPtr(CFuncType func) : m_func(func) { }
    FuncPtr(FuncPtr const& rhs) = default;
    FuncPtr& operator=(FuncPtr const& rhs) = default;

    Return operator()(ArgType... args) const
    {
        return m_func(std::forward<ArgType>(args)...);
    }
};

using AllocState = void;
using Address = void*;

class Allocator
{
  public:
    using AllocFcn = FuncPtr<Address(AllocState*, size_t)>;
    using DeallocFcn = FuncPtr<void(AllocState*, Address)>;
    using TeardownFcn = FuncPtr<bool(AllocState*)>;
  private:
    AllocState* m_state;
    AllocFcn m_alloc;
    DeallocFcn m_free;
    TeardownFcn m_teardown;
  public:
    Allocator(AllocState* state, AllocFcn alloc, DeallocFcn dealloc, TeardownFcn teardown)
      : m_state(state), m_alloc(alloc), m_free(dealloc), m_teardown(teardown)
    { }

    ~Allocator()
    {
        if (!m_state) return;
        bool success = m_teardown(m_state);
        assert(success); (void)success;
    }

    Allocator(Allocator const&) = delete;
    Allocator& operator=(Allocator const&) = delete;

    Allocator(Allocator&& rhs)
      : m_state(rhs.m_state), m_alloc(rhs.m_alloc), m_free(rhs.m_free), m_teardown(rhs.m_teardown)
    {
        rhs.m_state = nullptr;
    }

    Allocator& operator=(Allocator&& rhs)
    {
        if (this == &rhs) return *this;
        std::swap(m_state, rhs.m_state);
        std::swap(m_alloc, rhs.m_alloc);
        std::swap(m_free, rhs.m_free);
        std::swap(m_teardown, rhs.m_teardown);
        return *this;
    }

    Address allocate(size_t size) const { return m_alloc(m_state, size); }

    void deallocate(Address ptr) const { m_free(m_state, ptr); }
};

DT_API
/*!
 *
 */
Allocator const& defaultAllocator();

DT_API
/*!
 *
 */
Allocator createArenaAllocator(size_t defaultSize = 4096, Allocator const& baseAlloc = defaultAllocator());

} // namespace dt

#endif//_DATATOOLS_ALLOCATOR_HPP_
