/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "reactor.hpp"
#include "io_context.hpp"

Reactor::Reactor() : __moved(0)
{
    printf("reactor created\n");
}

Reactor::Reactor(Reactor&& r): __ep(std::move(r.__ep)),__moved(0)
{
    r.__moved = 1;
}

Reactor::~Reactor() {
    if(!__moved) printf("reactor destoryed\n");
}

void Reactor::addIOContext(IOContext *ioc)
{
    assert(!__moved);
    __ep.addIOContext(ioc);
}

void Reactor::delIOContext(IOContext *ioc)
{
    assert(!__moved);
    __ep.delIOContext(ioc);
}

void Reactor::loop()
{
    assert(!__moved);
    while (1)
    {
        auto io_contextes = __ep.poll();
        for (auto &ioc : io_contextes)
        {
            printf("point 1\n");
            //assert(ioc->isET());
            if (ioc->isReadable()) {
                printf("point 2\n");
                ioc->handleReadable();
            }
            if (ioc->isWritable())
                ioc->handleWritable();
        }
    }
}
