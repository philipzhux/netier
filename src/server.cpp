/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "server.hpp"
#include <iostream>

Server::Server(Address addr) : __addr(addr), __moved(0)
{
    __acceptor = std::make_unique<Acceptor>(__addr, std::bind(&Server::contextCreator, this, std::placeholders::_1, std::placeholders::_2));
    __threadpool = std::make_unique<ThreadPool>(5);
    size_t size = __threadpool->getSize();
    for (size_t i = 0; i < size; i++)
    {
        Reactor reactor{};
        __reactors.push_back(std::move(reactor));
    }
}

// Server::Server(Server &&other) : __addr(std::move(other.__addr)),
//                                  __acceptor(std::move(other.__acceptor)),
//                                  __threadpool(std::move(other.__threadpool)),
//                                  __reactors(std::move(other.__reactors)),
//                                  __contextes(std::move(other.__contextes)),
//                                  __moved(0) { other.__moved = 1; }

void Server::serve()
{
    size_t size = __threadpool->getSize();
    for (size_t i = 0; i < size; i++)
        __threadpool->asyncRunJob(std::bind(&Reactor::loop, &__reactors[i]));
    std::cout << "Listening on "
              << __addr.getAddressString() << " ..." << std::endl;
    __acceptor->getMainLoop()();
}

const Context &Server::contextCreator(int fd, Address addr)
{
    std::unique_ptr<Context> newContext =
        std::make_unique<Context>(fd, std::move(addr), schedule(fd),
                                  __onConn, std::bind(&Server::contextDestroyer, this, std::placeholders::_1));
    if (__onRecv)
        newContext->setOnRecv(__onRecv);
    __contextes[fd] = std::move(newContext);
    return *__contextes[fd].get();
}

void Server::contextDestroyer(int fd)
{
    __contextes.erase(fd);
}

Reactor *Server::schedule(int fd)
{
    return &__reactors[fd % __reactors.size()];
}

void Server::setOnConn(std::function<void(Context *)> onConn)
{
    __onConn = std::move(onConn);
}

void Server::setOnRecv(std::function<void(Context *)> onRecv)
{
    __onRecv = std::move(onRecv);
}

Server::~Server()
{
    for (auto &t : __contextes)
    {
        __contextes.erase(t.first);
    }
}
