/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#pragma once
#include "cppserv.h"
#include "acceptor.h"
#include <vector>
#include <unordered_map>
class Server
{
public:
    Server(Address addr);
    void setOnConn(std::function<Context *(void)>);
    void setOnRecv(std::function<Context *(void)>);
    void start();
    Context *contextCreator(int fd);
    void contextDestroyer(int fd);
    ~Server();

private:
    Address __addr;
    std::vector<Reactor> __reactors;
    std::unique_ptr<ThreadPool> __threadpool;
    std::unique_ptr<Acceptor> __acceptor;
    std::unordered_map<int, std::unique_ptr<Context>> contextes; // a context is binded with a reactor at create time
};