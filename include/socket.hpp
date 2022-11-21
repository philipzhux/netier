/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */
#pragma once
#include "address.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_EVENTS 1024

class Socket
{
private:
    int fd;

public:
    Socket();
    Socket(int);
    Socket(bool);
    Socket(Socket &&);
    int accept(Address &addr);
    int getFd() const;
    void listen();
    void bind(const Address &);
    void setBlocking(bool);
    size_t recvBufSize() const;
    ~Socket();
};