/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "socket.hpp"
#include "error.hpp"
#include <sys/ioctl.h>

Socket::Socket() {
    fd = socket(AF_INET,SOCK_STREAM,0);
    //printf("socket with fd %d initialized.\n",fd);
}

Socket::Socket(int ufd):fd(ufd){}

Socket::~Socket() {
    close(fd);
    //printf("socket[%d] destoryed.\n",fd);
}

void Socket::bind(const Address& addr) {
    errif(::bind(fd,(const sockaddr*)&(addr.getAddr()),sizeof(addr.getAddr()))<0,"bind");
    //printf("socket with fd %d binded with %s:%d.\n",fd,addr.getHostString().c_str(),addr.getPort());
}

size_t Socket::recvBufSize() const {
  size_t size = -1;
  if (::ioctl(fd, FIONREAD, &size) == -1) {
    perror("Socket get recv buf size failed");
  }
  return size;
}

void Socket::listen() {
    errif(::listen(fd,MAX_EVENTS)<0,"listen");
}

int Socket::accept(Address& addr){
    int ufd = ::accept(fd,addr.getAddrPtr(),addr.getAddrLenPtr());
    errif(ufd<0,"accept");
    return ufd;
}



int Socket::getFd() const{
    return fd;
}

void Socket::setBlocking(bool block) {
    if(block){
        ::fcntl(fd,F_SETFL,::fcntl(fd,F_GETFL)&(~O_NONBLOCK));
        return;
    }
    ::fcntl(fd,F_SETFL,::fcntl(fd,F_GETFL)&O_NONBLOCK);
}

Socket::Socket(bool block):Socket() {
    setBlocking(block);
}



