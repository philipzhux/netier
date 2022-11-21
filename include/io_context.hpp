/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */
#pragma once
#include <functional>
#include "reactor.hpp"

class Epoll;
/*
 * IOContext encapsulates an fd to be monitored by epoll
 * context of a tcp connection shall be further isolated
 * as a seperate class; Callbacks will be set by connection
 * as an std::function<void()> with [capture] *Context
 */

class IOContext
{
private:
    Reactor *__reactor;
    int __fd;
    uint32_t __events;
    uint32_t __revents; // return events
    std::function<void()> __writableCb;
    std::function<void()> __readableCb;
    bool __registered;

public:
    IOContext(Reactor *, int);
    IOContext(IOContext &&);
    IOContext(const IOContext &) = delete;
    ~IOContext();
    void enableRead();
    void enableWrite();
    void disableRead();
    void disableWrite();
    void setET();
    void setLT();
    void setEvents(uint32_t ev);
    void setRevents(uint32_t ret);
    void setReadCallback(std::function<void(void)>);
    void setWriteCallback(std::function<void(void)>);
    void handleReadable();
    void handleWritable();
    void handleGeneral();
    uint32_t getEvents();
    uint32_t getRevents();
    int getFd();
    bool isRegistered();
    bool isWritable();
    bool isReadable();
    bool isET();
    void setRegister();
};