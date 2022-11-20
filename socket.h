#ifndef SOCKET_H
#define SOCKET_H
#include "addr.h"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_EVENTS 1024

class Socket {
    private:
    int fd;
    public:
    Socket();
    Socket(int);
    Socket(bool);
    int accept(InetAddress& addr);
    int getFd() const;
    void listen();
    void bind(InetAddress&);
    void setBlocking(bool);
    size_t recvBufSize() const;
    ~Socket();
};
#endif