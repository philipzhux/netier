/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#pragma once
#include <arpa/inet.h>
#include <string>
#include <string.h>

namespace netier {


class Address
{

public:
    enum Address_type
    {
        IPV4 = AF_INET,
        IPV6 = AF_INET6
    };
    Address();
    Address(Address &&);
    Address(const Address &);
    Address(std::string host, uint16_t port, Address_type addrType);
    Address(std::string host, uint16_t port);
    Address &operator=(Address &&);
    Address &operator=(const Address &);
    const struct sockaddr_in &getAddr() const;
    const socklen_t &getAddrLen() const;
    sockaddr *getAddrPtr();
    socklen_t *getAddrLenPtr();
    std::string getHostString() const;
    std::string getAddressString() const;
    uint16_t getPort() const;
    void setAddress(std::string ip, uint16_t port);
    void setAddress(std::string ip, uint16_t port, Address_type type);

private:
    struct sockaddr_in __addr;
    socklen_t __addrLen;
    Address_type __address_type;
};
} // namespace netier
