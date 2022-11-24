/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "socket.hpp"
#include "error.hpp"
#include <assert.h>
#include <sys/ioctl.h>

namespace netier {

Socket::Socket() {
  fd = socket(AF_INET, SOCK_STREAM, 0);
  // printf("socket with fd %d initialized.\n",fd);
}

Socket::Socket(int ufd) : fd(ufd) {}

Socket::Socket(Socket &&other) : fd(other.fd) { other.fd = -1; }

Socket::~Socket() {
  if (fd > 0)
    close(fd);
  // printf("socket[%d] destroyed.\n",fd);
}

void Socket::bind(const Address &addr) {
  assert(fd > 0);
  errif(::bind(fd, (const sockaddr *)&(addr.getAddr()),
               sizeof(addr.getAddr())) < 0,
        "bind");
  // printf("socket with fd %d binded with
  // %s:%d.\n",fd,addr.getHostString().c_str(),addr.getPort());
}

size_t Socket::recvBufSize() const {
  assert(fd > 0);
  size_t size = -1;
  if (::ioctl(fd, FIONREAD, &size) == -1) {
    perror("Socket get recv buf size failed");
  }
  return size;
}

void Socket::listen() {
  assert(fd > 0);
  errif(::listen(fd, MAX_EVENTS) < 0, "listen");
}

int Socket::accept(Address &a) {
  assert(fd > 0);
  socklen_t len;
  struct sockaddr_storage addr;
  int ufd = ::accept(fd, (struct sockaddr *)&addr, &len);
  // getpeername(ufd, (struct sockaddr *)&addr, ;
  a.setAddress(reinterpret_cast<const struct sockaddr &>(addr));
  errif(ufd < 0, "accept");
  return ufd;
}

int Socket::getFd() const {
  assert(fd > 0);
  return fd;
}

void Socket::setBlocking(bool block) {
  assert(fd > 0);
  if (block) {
    ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) & (~O_NONBLOCK));
    return;
  }
  ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) & O_NONBLOCK);
}

Socket::Socket(bool block) : Socket() {
  assert(fd > 0);
  setBlocking(block);
}

} // namespace netier
