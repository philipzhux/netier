#include "socket.h"
#include "error.h"

Socket::Socket() {
    fd = socket(AF_INET,SOCK_STREAM,0);
    printf("socket with fd %d initialized.\n",fd);
}

Socket::Socket(int ufd):fd(ufd){}

Socket::~Socket() {
    close(fd);
    printf("socket[%d] destoryed.\n",fd);
}

void Socket::bind(InetAddress& addr) {
    errif(::bind(fd,(sockaddr*)&addr.getAddr(),sizeof(addr.getAddr()))<0,"bind");
    printf("socket with fd %d binded with %s:%d.\n",fd,addr.getIPString().c_str(),addr.getPort());
}

void Socket::listen() {
    errif(::listen(fd,MAX_EVENTS)<0,"listen");
}

int Socket::accept(InetAddress& addr){
    int ufd = ::accept(fd,(sockaddr*)&addr.getAddr(),&addr.getAddrLen());
    errif(ufd<0,"accept");
    return ufd;
}

int Socket::getFd() const{
    return fd;
}

void Socket::setBlocking(bool block) {
    if(block){
        ::fcntl(fd,F_SETFL,::fcntl(fd,F_GETFL)&(~O_NONBLOCK));
        return;
    }
    ::fcntl(fd,F_SETFL,::fcntl(fd,F_GETFL)&O_NONBLOCK);
}

Socket::Socket(bool block):Socket() {
    setBlocking(block);
}



