/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#pragma once
#include <functional>
#include <vector>
#include <assert.h>
#include "io_context.h"

/*
 * Reactor wraps a group of tasks to be handled each
 * reactor is associated with one thread and one epoll;
 * the existence of reactor is for fully utilize a thread
 * to handle a group of I/O events (I/O multiplex +
 * multitthreading).
 */

class Reactor
{
private:
    Epoll __ep;

public:
    Reactor();
    ~Reactor();
    void addIOContext(IOContext *);
    void Reactor::delIOContext(IOContext *);
    // add/modify context; auto removed when context is destroyed
    void loop();
    // an infinite loop
};