/*! mapfile.cpp */

#include "datatools/mapfile.hpp"
#include "datatools/allocator.hpp"

#include "MemoryMapped.h"

namespace dt
{

class MappedFile::Impl : public MemoryMapped
{
};

size_t const MappedFile::EntireFile = ~static_cast<size_t>(0);

MappedFile::MappedFile()
  : m_impl(nullptr)
{
    Address mem = defaultAllocator().allocate(sizeof(Impl));
    if (mem) {
        m_impl = new (mem) Impl();
    }
}

MappedFile::~MappedFile()
{
    if (m_impl) {
        m_impl->~Impl();
        defaultAllocator().deallocate(m_impl);
    }
}

bool MappedFile::isOpen() const
{
    return m_impl && m_impl->isValid();
}

size_t MappedFile::fileSize() const
{
    assert(m_impl);
    return m_impl->size();
}

bool MappedFile::open(CString file, size_t mapSize, AccessPattern::Type accessPatt)
{
    MemoryMapped::CacheHint hint;
    switch (accessPatt) {
        case AccessPattern::DEFAULT:
            hint = MemoryMapped::Normal;
            break;
        case AccessPattern::SEQUENTIAL:
            hint = MemoryMapped::SequentialScan;
            break;
        case AccessPattern::RANDOM:
            hint = MemoryMapped::RandomAccess;
            break;
    }

    size_t sz = mapSize == EntireFile ? 0 : mapSize;

    return m_impl->open(file, sz, hint);
}

void MappedFile::close()
{
    if (m_impl) {
        m_impl->close();
    }
}

size_t MappedFile::mapSize() const
{
    assert(m_impl);
    return m_impl->mappedSize();
}

/* offset must be a multiple of 4096 */
bool MappedFile::remap(uint64_t offset, size_t mappedBytes)
{
    assert(m_impl);
    return m_impl->remap(offset, mappedBytes);
}

CString MappedFile::getData() const
{
    return m_impl ? reinterpret_cast<CString>(m_impl->getData()) : 0;
}

} // namespace dt
