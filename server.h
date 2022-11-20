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
#include <string>
class Server
{
public:
    // Server(std::string host, uint16_t port);
    Server(Address addr);
    ~Server();
    void setOnConn(std::function<Context *(void)>);
    void setOnRecv(std::function<Context *(void)>);
    void serve();
    Context *contextCreator(int fd, Address addr);
    Reactor* schedule(int fd);
    void contextDestroyer(int fd);
   

private:
    Address __addr;
    std::vector<Reactor> __reactors;
    std::unique_ptr<ThreadPool> __threadpool;
    std::unique_ptr<Acceptor> __acceptor;
    std::unordered_map<int, std::unique_ptr<Context>> __contextes; // a context is binded with a reactor at create time
    std::function<Context *(void)> __onConn;
    std::function<Context *(void)> __onRecv;
};