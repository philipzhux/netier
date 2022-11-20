/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */
#include "acceptor.h"

Acceptor::Acceptor(Address& address, std::function<Context&(int)> contextCreator): 
        __contextCreator(contextCreator), __address(address)
{
    __socket = std::make_unique<Socket>();
    __socket->bind(__address);
    __ioc = std::make_unique<IOContext>(&__reactor, __socket->getFd());
    __ioc->enableRead();
    __ioc->setReadCallback(std::bind(&Acceptor::AcceptConnection,this));
    __socket->listen();
}


void Acceptor::AcceptConnection() {
    Address addr;
    int cfd = __socket->accept(addr);
    fcntl(cfd, F_SETFL, fcntl(cfd, F_GETFL) | O_NONBLOCK);
    Context& context = __contextCreator(cfd);
    context.getAddress() = std::move(addr);
}


const Address& Acceptor::getAddress() {
    return __address;
}

std::function<void(void)> Acceptor::getMainLoop() {
    return std::bind(&(Reactor::loop),&(this->__reactor));
}