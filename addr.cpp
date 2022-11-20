/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "addr.h"
Address::Address(std::string ip, uint16_t port) : Address()
{
        setAddress(ip, port);
}

Address::Address()
{
        memset(&addr, 0, sizeof(addr));
        memset(&addrLen, 0, sizeof(addrLen));
}

const struct sockaddr_in &Address::getAddr() const
{
        return addr;
}

const socklen_t &Address::getAddrLen() const
{
        return addrLen;
}

sockaddr* Address::getAddrPtr()
{
        return (sockaddr*)&addr;
}

socklen_t *Address::getAddrLenPtr()
{
        return &addrLen;
}

void Address::setAddress(std::string ip, uint16_t port)
{
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);
        addrLen = sizeof(addr);
}

std::string Address::getIPString()
{
        return ::inet_ntoa(addr.sin_addr);
}

uint16_t Address::getPort()
{
        return ::ntohs(addr.sin_port);
}