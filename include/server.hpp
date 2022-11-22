/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#pragma once
#include "cppserv.hpp"
#include "context.hpp"
#include "address.hpp"
#include "acceptor.hpp"
#include "threadpool.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

namespace netier {


class Server
{
public:
    // Server(std::string host, uint16_t port);
    Server(Address);
    Server(const Server &) = delete;
    Server(Server &&) = delete;
    Server &operator=(Server &&) = delete;
    Server &operator=(const Server &) = delete;
    ~Server();
    void setOnConn(std::function<void(Context *)>);
    void setOnRecv(std::function<void(Context *)>);
    void serve();
    const Context &contextCreator(int fd, Address addr);
    Reactor *schedule(int fd);
    void contextDestroyer(int fd);

private:
    Address __addr;
    std::vector<Reactor> __reactors;
    std::unique_ptr<ThreadPool> __threadpool;
    std::unique_ptr<Acceptor> __acceptor;
    std::unordered_map<int, std::unique_ptr<Context>> __contextes; // a context is binded with a reactor at create time
    std::function<void(Context *)> __onConn;
    std::function<void(Context *)> __onRecv;
    int __moved;
};
} // namespace netier
