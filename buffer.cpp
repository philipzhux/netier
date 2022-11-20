#include "buffer.h"
#include <assert.h>

void *Buffer::getWriter(size_t size)
{
    assert(size > 0);
    size_t prev_size = __buffer.size();
    __buffer.resize(prev_size + size);
    return static_cast<void *>(&__buffer[prev_size]);
}

void Buffer::put(const void *buf, size_t size)
{
    __buffer.insert(__buffer.end(), (const char *)buf, (const char *)(buf + size));
}

std::vector<char> Buffer::get(size_t size)
{
    std::vector<char> v(__buffer.begin(), __buffer.begin() + size);
    __buffer.erase(__buffer.begin(), __buffer.begin() + size);
    return std::move(v);
}

const void *Buffer::getReader()
{
    return static_cast<const void *>(__buffer.data());
}

void Buffer::moveReadCursor(size_t size)
{
    __buffer.erase(__buffer.begin(), __buffer.begin() + size);
}

std::vector<char> Buffer::get()
{
    std::vector<char> v = __buffer;
    clear();
    return std::move(v);
}

const std::vector<char> &Buffer::getInnerVec()
{
    return __buffer;
}

void Buffer::clear()
{
    __buffer.clear();
}

void Buffer::shrink(size_t size)
{
    assert(size <= __buffer.size());
    __buffer.resize(__buffer.size() - size);
}

size_t Buffer::size()
{
    return __buffer.size();
}
