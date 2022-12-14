/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "buffer.hpp"
#include <assert.h>
#include <stdio.h>

namespace netier {

Buffer::Buffer() {}
Buffer::~Buffer() {}

Buffer::Buffer(Buffer &&other) : __buffer(std::move(other.__buffer)) {}

Buffer &Buffer::operator=(Buffer &&other) {
  if (&other == this)
    return *this;
  __buffer = std::move(other.__buffer);
  return *this;
}

void *Buffer::getWriter(size_t size) {
  assert(size > 0);
  size_t prev_size = __buffer.size();
  __buffer.resize(prev_size + size);
  return static_cast<void *>(&__buffer[prev_size]);
}

void Buffer::put(const void *buf, size_t size) {
  __buffer.insert(__buffer.end(), (const char *)buf, (const char *)buf + size);
}

std::vector<char> Buffer::get(size_t size) {
  std::vector<char> v(__buffer.begin(), __buffer.begin() + size);
  __buffer.erase(__buffer.begin(), __buffer.begin() + size);
  return v;
}

const void *Buffer::getReader() {
  return static_cast<const void *>(__buffer.data());
}

void Buffer::moveReadCursor(size_t size) {
  __buffer.erase(__buffer.begin(), __buffer.begin() + size);
}

std::vector<char> Buffer::get() {
  std::vector<char> v = __buffer;
  clear();
  return v;
}

const std::vector<char> &Buffer::getInnerVec() { return __buffer; }

void Buffer::clear() { __buffer.clear(); }

void Buffer::shrink(size_t size) {
  // printf("Buffer::shrink(): size of buffer = %ld\n",__buffer.size());
  // printf("Buffer::shrink(): trying to shrink to size %ld\n",__buffer.size() -
  // size);
  assert(size <= __buffer.size());
  __buffer.resize(__buffer.size() - size);
}

size_t Buffer::size() { return __buffer.size(); }

} // namespace netier
