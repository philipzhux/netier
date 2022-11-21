/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "address.hpp"

Address::Address(std::string host, uint16_t port, Address_type addrType) : Address()
{
        __address_type = addrType;
        setAddress(host, port);
}
Address::Address(std::string host, uint16_t port) : Address()
{
        setAddress(host, port);
}

Address::Address() : __address_type(IPV4)
{
        memset(&__addr, 0, sizeof(__addr));
        memset(&__addrLen, 0, sizeof(__addrLen));
}

Address::Address(Address &&other) : __address_type(std::move(other.__address_type)),
                                   __addr(std::move(other.__addr)),
                                   __addrLen(std::move(other.__addrLen)) {}

Address::Address(const Address &other) : __address_type(other.__address_type),
                                   __addr(other.__addr),
                                   __addrLen(other.__addrLen) {}                    

Address& Address::operator=(Address&& other)
{
        if(this==&other) return *this;
        __address_type = std::move(other.__address_type);
        __addr = std::move(other.__addr);
        __addrLen = std::move(other.__addrLen);
        return *this;

}
Address& Address::operator=(const Address& other)
{
        if(this==&other) return *this;
        __address_type = other.__address_type;
        __addr = other.__addr;
        __addrLen = other.__addrLen;
        return *this;
}

const struct sockaddr_in &Address::getAddr() const
{
        return __addr;
}

const socklen_t &Address::getAddrLen() const
{
        return __addrLen;
}

sockaddr *Address::getAddrPtr()
{
        return (sockaddr *)&__addr;
}

socklen_t *Address::getAddrLenPtr()
{
        return &__addrLen;
}

void Address::setAddress(std::string ip, uint16_t port)
{
        __addr.sin_addr.s_addr = inet_addr(ip.c_str());
        __addr.sin_family = __address_type;
        __addr.sin_port = htons(port);
        __addrLen = sizeof(__addr);
}

std::string Address::getHostString() const
{
        return ::inet_ntoa(__addr.sin_addr);
}

uint16_t Address::getPort() const
{
        return ::ntohs(__addr.sin_port);
}
