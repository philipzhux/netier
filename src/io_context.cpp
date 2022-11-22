/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "io_context.hpp"

namespace netier {

IOContext::IOContext(Reactor *rt, int fd)
    : __reactor(rt), __fd(fd), __registered(false), __events(0), __revents(0) {}

IOContext::IOContext(IOContext &&old)
    : __reactor(old.__reactor), __fd(old.__fd), __registered(old.__registered),
      __writableCb(std::move(old.__writableCb)),
      __readableCb(std::move(old.__readableCb)), __events(old.__events),
      __revents(old.__revents) {
  old.__reactor = nullptr;
  old.__fd = -1;
}

IOContext::~IOContext() {
  if (__reactor) {
    // printf("destryoed ioContext");
    __reactor->delIOContext(this);
  }
}

void IOContext::enableRead() {
  assert(__reactor);
  setEvents(getEvents() | EPOLLIN);
}

void IOContext::disableRead() {
  assert(__reactor);
  setEvents(getEvents() & ~EPOLLIN);
}

void IOContext::enableWrite() {
  assert(__reactor);
  setEvents(getEvents() | EPOLLOUT);
}

void IOContext::disableWrite() {
  assert(__reactor);
  setEvents(getEvents() & ~EPOLLOUT);
}

void IOContext::setET() {
  assert(__reactor);
  setEvents(getEvents() | EPOLLET);
}

void IOContext::setLT() {
  assert(__reactor);
  setEvents(getEvents() & ~EPOLLET);
}

void IOContext::setEvents(uint32_t ev) {
  assert(__reactor);
  __events = ev;
  __reactor->addIOContext(this);
}

void IOContext::setRevents(uint32_t ret) {
  assert(__reactor);
  __revents = ret;
}

bool IOContext::isET() {
  assert(__reactor);
  return (getEvents() & EPOLLET) != 0;
}

bool IOContext::isWritable() {
  assert(__reactor);
  return (getRevents() & EPOLLOUT) != 0;
}

bool IOContext::isReadable() {
  if (!__reactor)
    return false;
  assert(__reactor);
  return (getRevents() & EPOLLIN) != 0;
}

uint32_t IOContext::getEvents() {
  assert(__reactor);
  return __events;
}

uint32_t IOContext::getRevents() {
  assert(__reactor);
  return __revents;
}

int IOContext::getFd() {
  assert(__reactor);
  return __fd;
}
bool IOContext::isRegistered() { return __registered; }

void IOContext::setRegister() {
  assert(__reactor);
  __registered = true;
}

void IOContext::setReadCallback(std::function<int(void)> cb) {
  assert(__reactor);
  __readableCb = cb;
}

void IOContext::setWriteCallback(std::function<int(void)> cb) {
  assert(__reactor);
  __writableCb = cb;
}

int IOContext::handleWritable() {
  assert(__reactor);
  if (__writableCb) {
    return __writableCb();
  }
  return 0;
}

int IOContext::handleReadable() {
  assert(__reactor);
  if (__readableCb) {
    return __readableCb();
  }
  return 0;
}

void IOContext::handleGeneral() {
  if (isReadable()) {
    if (handleReadable() < 0) {
      return;
    }
  }
  if (isWritable()) {
    if (handleWritable() < 0) {
      return;
    }
  }
}
} // namespace netier
