/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#pragma once
#include <arpa/inet.h>
#include <string>
#include <string.h>

class Address {
    private:
    struct sockaddr_in addr;
    socklen_t addrLen;

    public:
    Address();
    Address(std::string host,uint16_t port);
    const struct sockaddr_in & getAddr() const;
    const socklen_t & getAddrLen() const;
    sockaddr* getAddrPtr();
    socklen_t* getAddrLenPtr();
    std::string getHostString() const;
    uint16_t getPort() const;
    void setAddress (std::string ip,uint16_t port);
};