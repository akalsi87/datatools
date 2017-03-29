/*! mapfile.hpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#ifndef _DATATOOLS_MAPFILE_HPP_
#define _DATATOOLS_MAPFILE_HPP_

#include "version.hpp"

#include <utility>

namespace dt
{

class DT_API MappedFile
{
  private:
    class Impl;

    Impl* m_impl;
  public:
    struct AccessPattern
    {
        enum Type
        {
            DEFAULT,
            SEQUENTIAL,
            RANDOM
        };
    };

    static size_t const EntireFile;

    MappedFile();

    MappedFile(MappedFile const&) = delete;
    MappedFile& operator=(MappedFile const&) = delete;

    MappedFile(MappedFile&& rhs) : m_impl(rhs.m_impl)
    {
        rhs.m_impl = nullptr;
    }

    MappedFile& operator=(MappedFile&& rhs)
    {
        if (this != &rhs) {
            std::swap(m_impl, rhs.m_impl);
        }
        return *this;
    }

    ~MappedFile();

    bool isOpen() const;
    size_t fileSize() const;
    bool open(CString file, size_t mapSize = EntireFile, AccessPattern::Type accessPatt = AccessPattern::DEFAULT);
    void close();

    size_t mapSize() const;

    /* offset must be a multiple of 4096 */
    bool remap(uint64_t offset, size_t mappedBytes);

    CString getData() const;
};

} // namespace dt

#endif//_DATATOOLS_MAPFILE_HPP_
