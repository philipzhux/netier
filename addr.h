#ifndef ADDR_H
#define ADDR_H
#include <arpa/inet.h>
#include <string>
#include <string.h>

class InetAddress {
    private:
    struct sockaddr_in addr;
    socklen_t addrLen;

    public:
    InetAddress();
    InetAddress(std::string ip,uint16_t port);
    struct sockaddr_in & getAddr();
    socklen_t & getAddrLen();
    std::string getIPString();
    uint16_t getPort();
    void setAddress (std::string ip,uint16_t port);
};
#endif