#ifndef EPOLL_H
#define EPOLL_H
#include <sys/epoll.h>
#include <unistd.h>
#include "socket.h"
#include "error.h"
#include "channel.h"
#include <vector>
#define MAX_EVENTS 1024

class Epoll {
    private:
    int epfd;
    epoll_event* events;
    int waitEvents();
    epoll_event* getEvents();

    public:
    Epoll();
    ~Epoll();
    std::vector<Channel*> poll();
    void updateChan(Channel* chan);
    void add(Channel* chan,uint32_t events);
    void add(int fd,uint32_t events);
    void add(int fd);
    void add(Socket const& socket,uint32_t events);
    void add(Socket const& socket);
    void modify(int fd,uint32_t events);
    void modify(Channel* chan,uint32_t events);
    void del(int fd);

};
#endif
