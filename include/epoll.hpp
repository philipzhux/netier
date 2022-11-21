/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include "socket.hpp"
#include "error.hpp"
#include <vector>
#include <memory>
#define MAX_EVENTS 1024

class IOContext;

class Epoll
{
private:
    int epfd;
    std::unique_ptr<epoll_event[]> events;
    //epoll_event *events;
    int waitEvents();
    epoll_event *getEvents();

public:
    Epoll();
    Epoll(Epoll&&);
    Epoll(const Epoll&) = delete;
    ~Epoll();
    std::vector<IOContext *> poll();
    void addIOContext(IOContext *ioc);
    void delIOContext(IOContext *ioc);
    void add(IOContext *ioc, uint32_t events);
    void add(int fd, uint32_t events);
    void add(int fd);
    void add(Socket const &socket, uint32_t events);
    void add(Socket const &socket);
    void modify(int fd, uint32_t events);
    void modify(IOContext *c, uint32_t events);
    void del(int fd);
};

