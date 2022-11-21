/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */
#pragma once
#include "cppserv.hpp"
#include "reactor.hpp"
#include "address.hpp"
#include "socket.hpp"
#include "io_context.hpp"
#include <memory>
#include <functional>
class Context;

class Acceptor
{
public:
    Acceptor(Address &address, std::function<const Context &(int, Address)> contextCreator);
    Acceptor(Acceptor &&);
    Acceptor(const Acceptor &) = delete;
    Acceptor &operator=(Acceptor &&) = delete;
    ~Acceptor();
    void AcceptConnection();
    const Address &getAddress();
    std::function<void(void)> getMainLoop();

private:
    const Address &__address;
    Reactor __reactor;
    std::unique_ptr<IOContext> __ioc;
    std::unique_ptr<Socket> __socket;
    std::function<const Context &(int, Address)> __contextCreator;
};