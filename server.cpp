/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "server.h"

Server::Server(Address addr) : __addr(addr)
{
    __acceptor = std::make_unique<Acceptor>(__addr, std::bind(&Server::contextCreator, this));
    __threadpool = std::make_unique<ThreadPool>();
    size_t size = __threadpool->getSize();
    for (size_t i = 0; i < size; i++)
        __reactors.emplace_back(Reactor{});
}

void Server::serve()
{
    size_t size = __threadpool->getSize();
    for (size_t i = 0; i < size; i++)
        __threadpool->asyncRunJob(std::bind(&Reactor::loop,&__reactors[i]));
    

}



void Server::setOnConn(std::function<Context *(void)> onConn)
{
    __onConn = std::move(onConn);
}

void Server::setOnRecv(std::function<Context *(void)> onRecv)
{
    __onRecv = std::move(onRecv);
}
