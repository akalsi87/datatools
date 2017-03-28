/*! array.hpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#ifndef _DATATOOLS_ARRAY_HPP_
#define _DATATOOLS_ARRAY_HPP_

#include "version.hpp"
#include "allocator.hpp"

#include <cstdlib>
#include <new>

namespace dt
{

template <class Elem>
class Array
{
  public:
    Array(size_t n = 0, Allocator const& a = defaultAllocator()) : m_array(nullptr), m_size(0), m_alloc(&a)
    {
        resize(n);
    }

    ~Array()
    {
        if (m_array) { m_alloc->deallocate(m_array); }
    }       

    Array(Array const&) = delete;
    Array& operator=(Array const&) = delete;

    Array(Array&& rhs) : m_array(rhs.m_array), m_size(rhs.m_size), m_alloc(rhs.m_alloc)
    {
        rhs.m_array = nullptr;
        rhs.m_size = 0;
    }

    Array& operator=(Array&& rhs)
    {
        if (this == &rhs) return *this;
        if (m_array) {
            m_alloc->deallocate(m_array);
        }
        m_array = rhs.m_array;
        m_alloc = rhs.m_alloc;
        rhs.m_array = nullptr;
        rhs.m_size = 0;
        return *this;
    }

    Elem& operator[](size_t idx)
    {
        assert(idx < m_size);
        return m_array[idx];
    }

    Elem const& operator[](size_t idx) const
    {
        assert(idx < m_size);
        return m_array[idx];
    }

    size_t size() const { return m_size; }

    void reset(size_t n = 0)
    {
        if (m_array) { m_alloc->deallocate(m_array); }
        resize(n);
    }

  private:
    Elem* m_array;
    size_t m_size;
    Allocator const* m_alloc;

    void resize(size_t n)
    {
        if (n != 0) {
            m_array = (Elem*)m_alloc->allocate(n*sizeof(Elem));
            assert(m_array);
            m_size = n;
            for (size_t i = 0; i < n; ++i) {
                new (static_cast<void*>(&m_array[i])) Elem();
            }
        } else {
            m_size = 0;
            m_array = nullptr;
        }
    }
};

} // namespace dt

#endif//_DATATOOLS_ARRAY_HPP_
