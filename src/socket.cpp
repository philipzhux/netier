/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "socket.hpp"
#include "error.hpp"
#include <sys/ioctl.h>
#include <assert.h>

Socket::Socket()
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    // printf("socket with fd %d initialized.\n",fd);
}

Socket::Socket(int ufd) : fd(ufd) {}

Socket::Socket(Socket &&other) : fd(other.fd)
{
    other.fd = -1;
}

Socket::~Socket()
{
    if (fd > 0)
        close(fd);
    // printf("socket[%d] destroyed.\n",fd);
}

void Socket::bind(const Address &addr)
{
    assert(fd > 0);
    errif(::bind(fd, (const sockaddr *)&(addr.getAddr()), sizeof(addr.getAddr())) < 0, "bind");
    // printf("socket with fd %d binded with %s:%d.\n",fd,addr.getHostString().c_str(),addr.getPort());
}

size_t Socket::recvBufSize() const
{
    assert(fd > 0);
    size_t size = -1;
    if (::ioctl(fd, FIONREAD, &size) == -1)
    {
        perror("Socket get recv buf size failed");
    }
    return size;
}

void Socket::listen()
{
    assert(fd > 0);
    errif(::listen(fd, MAX_EVENTS) < 0, "listen");
}

int Socket::accept(Address &a)
{
    assert(fd > 0);
    socklen_t len;
    int port;
    struct sockaddr_storage addr;
    char ipstr[INET6_ADDRSTRLEN];

    int ufd = ::accept(fd, nullptr, nullptr);
    getpeername(ufd, (struct sockaddr *)&addr, &len);

    if (addr.ss_family == AF_INET)
    {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
        a.setAddress(ipstr, port);
    }
    else
    { // AF_INET6
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        port = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
        a.setAddress(ipstr, port, Address::Address_type::IPV6);
    }

    errif(ufd < 0, "accept");
    return ufd;
}

int Socket::getFd() const
{
    assert(fd > 0);
    return fd;
}

void Socket::setBlocking(bool block)
{
    assert(fd > 0);
    if (block)
    {
        ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) & (~O_NONBLOCK));
        return;
    }
    ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) & O_NONBLOCK);
}

Socket::Socket(bool block) : Socket()
{
    assert(fd > 0);
    setBlocking(block);
}
