/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */
#include "cppserv.h"

class Acceptor {
    public:
    Acceptor(Address& address, std::function<Context&(int)> contextCreator);
    ~Acceptor();
    void AcceptConnection();
    const Address& getAddress();
    std::function<void(void)> getMainLoop();
    private:
    const Address& __address;
    Reactor __reactor;
    std::unique_ptr<IOContext> __ioc;
    std::unique_ptr<Socket> __socket;
    std::function<Context&(int)> __contextCreator;
};