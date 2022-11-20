#include "reactor.h"
#include <assert.h>
Reactor::Reactor()
{
    __ep = Epoll();
}

Reactor::~Reactor()
{
}

void Reactor::addIOContext(IOContext *ioc)
{
    __ep.addIOContext(ioc);
}

void Reactor::delIOContext(IOContext *ioc)
{
    __ep.delIOContext(ioc);
}

void Reactor::loop()
{
    while (1)
    {
        auto io_contextes = __ep.poll();
        for (auto &ioc : io_contextes)
        {
            assert(ioc->isET());
            if (ioc->isReadable())
                ioc->handleReadable();
            if (ioc->isWritable())
                ioc->handleWritable();
        }
    }
}
