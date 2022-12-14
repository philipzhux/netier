/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "context.hpp"
#include <iostream>

namespace netier {

Context::Context(int cfd, Address address, Reactor *reactor,
                 std::function<void(Context *)> onConenct,
                 std::function<void(int)> onDestroy)
    : __socket(std::make_unique<Socket>(cfd)),
      __buffer(std::make_unique<Buffer>()),
      __wbuffer(std::make_unique<Buffer>()), __reactor(reactor),
      __destroyContext(onDestroy), __onConn(std::move(onConenct)),
      __address(std::move(address)) {
  assert(__destroyContext);
  __ioContext = std::make_unique<IOContext>(__reactor, cfd);
  __ioContext
      ->enableRead(); // register ioContext on reactor and therefore epoll
  __ioContext->enableWrite();
  __ioContext->setET();
  __ioContext->setReadCallback(std::bind(&Context::handleReadableEvent, this));
  __ioContext->setWriteCallback(std::bind(&Context::handleWritableEvent, this));

  __state = VALID;
  if (__onConn)
    __onConn(this);
}

Context::Context(Context &&other)
    : __ioContext(std::move(other.__ioContext)), __state(other.__state) {
  other.__state = MOVED;
}

Context::~Context() {}

int Context::handleWritableEvent() {
  assert(__state != MOVED);
  int ret;
  if ((ret = __stateCleanup()) < 0)
    return ret;
  flushWriteBuffer();
  return __stateCleanup();
}

void Context::flushWriteBuffer() {
  assert(__state != MOVED);
  if (__state != VALID)
    return;
  int bytes_written;
  int __socket_fd = __socket->getFd();
  while (__wbuffer->size() > 0) {
    bytes_written =
        ::write(__socket_fd, __wbuffer->getReader(), __wbuffer->size());
    if (bytes_written > 0) {
      __wbuffer->moveReadCursor(bytes_written);
    } else if (bytes_written == -1 &&
               (errno == EWOULDBLOCK || errno == EAGAIN)) {
      break;
    } else if (bytes_written == 0) {
      __state = CLOSED;
      printf(
          "[Context::flushWriteBuffer] Read EOF, client fd %d disconnected\n",
          __socket_fd);
      // this->destroy();
      break;
    } else {
      __state = INVALID;
      printf(
          "[Context::flushWriteBuffer] Unknown error on fd=%d, socket closed "
          "and context destroyed\n",
          __socket_fd);
      // this->destroy();
      break;
    }
  }
}

int Context::__stateCleanup() {
  if (__state == CLOSED || __state == INVALID) {
    this->destroy();
    return -1;
  }
  return 0;
}

int Context::handleReadableEvent() {
  assert(__state != MOVED);
  int ret;
  if ((ret = __stateCleanup()) < 0)
    return ret;
  int fd = __socket->getFd();
  int bytes_read;
  while (1) {
    void *writer = __buffer->getWriter(Context::read_buf_size);
    bytes_read = ::read(fd, writer, Context::read_buf_size);
    if (bytes_read > 0) {
      if (int(Context::read_buf_size) > bytes_read) {
        __buffer->shrink(Context::read_buf_size - bytes_read);
      }
    } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      __buffer->shrink(Context::read_buf_size);
      break;
    } else if (bytes_read == 0) {
      __buffer->shrink(Context::read_buf_size);
      __state = CLOSED;
      printf("[Context::handleReadableEvent] Read EOF, client fd %d "
             "disconnected\n",
             fd);
      break;
      // this->destroy();
      // return -1;
    } else {
      __buffer->shrink(Context::read_buf_size);
      __state = INVALID;
      printf("[Context::handleReadableEvent] Unknown error on fd=%d, socket "
             "closed and context destroyed\n",
             fd);
      break;
      // this->destroy();
      // return -1;
    }
  }
  // printf("Context::handleReadableEvent(): moved readable data to bufer\n");
  if ((ret = __stateCleanup()) < 0)
    return ret;
  if (__onRecv)
    __onRecv(this);
  return __stateCleanup();
}

void Context::asyncWrite(const void *buf, size_t size) {
  assert(__state != MOVED);
  if (__state != VALID)
    return;
  __wbuffer->put(buf, size);
  flushWriteBuffer();
}

ER Context::syncWrite(const void *buf, size_t size) {
  assert(__state != MOVED);
  if (__state != VALID)
    return ER::SOCKET_ERROR;
  int bytes_written;
  int remaining = size;
  int __socket_fd = __socket->getFd();
  while (remaining > 0) {
    bytes_written = ::write(__socket_fd, buf, remaining);
    if (bytes_written > 0) {
      // printf("Context::syncWrite: bytes_written = %d\n", bytes_written);
      remaining -= bytes_written;
    } else if (bytes_written == -1 &&
               (errno == EWOULDBLOCK || errno == EAGAIN)) {
      if (remaining > 0)
        asyncWrite((const char *)buf + (size - remaining), remaining);
      return ER::SUCCESS;
    } else if (bytes_written == 0) {
      __state = CLOSED;
      printf("[Context::syncWrite] Read EOF, client fd %d disconnected\n",
             __socket_fd);
      // this->destroy();
      return ER::SOCKET_ERROR;
    } else {
      __state = INVALID;
      printf("[Context::syncWrite] Unknown error on fd=%d, socket closed and "
             "context destroyed\n",
             __socket_fd);
      // this->destroy();
      return ER::SOCKET_ERROR;
    }
  }
  return ER::SUCCESS;
}

ER Context::write(const std::string &data) {
  assert(__state != MOVED);
  if (__state != VALID)
    return ER::SOCKET_ERROR;
  return syncWrite(data.c_str(), data.size() + 1);
}

ER Context::write(std::string &&data) {
  assert(__state != MOVED);
  if (__state != VALID)
    return ER::SOCKET_ERROR;
  return write(std::move(data));
}

ER Context::write(const std::vector<char> &data) {
  assert(__state != MOVED);
  if (__state != VALID)
    return ER::SOCKET_ERROR;
  return syncWrite(data.data(), data.size());
}

ER Context::write(std::vector<char> &&data) {
  assert(__state != MOVED);
  if (__state != VALID)
    return ER::SOCKET_ERROR;
  return write(std::move(data));
}

ER Context::writeFile(std::string filePath) {
  assert(__state != MOVED);
  if (__state != VALID)
    return ER::SOCKET_ERROR;
  FILE *file;
  int bytes_read;
  if ((file = ::fopen(filePath.c_str(), "r"))) {
    while (1) {
      bytes_read = fread(__wbuffer->getWriter(Context::read_buf_size),
                         sizeof(char), Context::read_buf_size, file);
      if (bytes_read < int(Context::read_buf_size)) {
        __wbuffer->shrink(Context::read_buf_size - bytes_read);
      }
      if (bytes_read == 0) {
        break;
      }
    }
    flushWriteBuffer();
    return ER::SUCCESS;
  } else {
    return ER::FILE_ERROR;
  }
}

ER Context::writeFile(std::string filePath, size_t size) {
  assert(__state != MOVED);
  if (__state != VALID)
    return ER::SOCKET_ERROR;
  if (filePath.size() == size)
    return ER::UNIMPLEMENTED;
  return ER::UNIMPLEMENTED;
}

const Address &Context::getAddress() {
  assert(__state != MOVED);
  return __address;
}

std::vector<char> Context::read() {
  assert(__state != MOVED);
  return std::move(__buffer->get());
}

std::string Context::readString() {
  assert(__state != MOVED);
  if (__state != VALID)
    return "";
  auto v = __buffer->getInnerVec();
  std::string ret(v.begin(), v.end());
  __buffer->clear();
  return ret;
}

Context::State Context::getState() {
  assert(__state != MOVED);
  return __state;
}

void Context::setOnRecv(std::function<void(Context *)> onRecv) {
  assert(__state != MOVED);
  __onRecv = std::move(onRecv);
}

void Context::destroy() {
  assert(__state != MOVED);
  std::cout << "[Context::destroy] Destroyed context "
            << getAddress().getAddressString() << std::endl;
  __destroyContext(__socket->getFd());
}
} // namespace netier
