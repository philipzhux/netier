#include "addr.h"
InetAddress::InetAddress(std::string ip,uint16_t port):InetAddress(){
        setAddress(ip,port);
}

InetAddress::InetAddress() {
        memset(&addr,0,sizeof(addr));
        memset(&addrLen,0,sizeof(addrLen));
}

struct sockaddr_in & InetAddress::getAddr() {
    return addr;
}

socklen_t & InetAddress::getAddrLen() {
        return addrLen;
}

void InetAddress::setAddress(std::string ip,uint16_t port){
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);
        addrLen = sizeof(addr);
}

std::string InetAddress::getIPString() {
        return ::inet_ntoa(addr.sin_addr);
}

uint16_t InetAddress::getPort() {
        return ::ntohs(addr.sin_port);
}