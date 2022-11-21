/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "epoll.hpp"
#include "io_context.hpp"

Epoll::Epoll()
{
    epfd = epoll_create1(0);
    events = std::make_unique<struct epoll_event[]>(MAX_EVENTS);
    // events = new struct epoll_event[MAX_EVENTS];
    //  printf("epoll with epfd  = %d initialized.\n",epfd);
}

Epoll::Epoll(Epoll &&e)
{
    epfd = e.epfd;
    events = std::move(e.events);
    e.epfd = -1;
}

Epoll::~Epoll()
{
    if (epfd >= 0)
    {
        close(epfd);
        // printf("epoll with epfd  = %d destroyed.\n",epfd);
    }
}

void Epoll::addIOContext(IOContext *c)
{
    if (!c->isRegistered())
    {
        add(c, c->getEvents());
        c->setRegister();
    }
    else
    {
        modify(c, c->getEvents());
    }
}

void Epoll::delIOContext(IOContext *c)
{
    del(c->getFd());
}

void Epoll::add(int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0, "Epoll::add");
}

void Epoll::add(IOContext *c, uint32_t events)
{
    struct epoll_event ev;
    ev.data.ptr = (void *)c;
    ev.events = events;
    // printf("epoll add monitered fd %d\n",c->getFd());
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, c->getFd(), &ev) < 0, "Epoll::add");
}

void Epoll::add(Socket const &socket, uint32_t events)
{
    struct epoll_event ev;
    ev.data.fd = socket.getFd();
    ev.events = events;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, socket.getFd(), &ev) < 0, "Epoll::add");
    // printf("epfd[%d] monitoring socket[%d]\n",epfd,socket.getFd());
}

void Epoll::add(Socket const &socket)
{
    add(socket, EPOLLIN | EPOLLET);
}

void Epoll::add(int fd)
{
    add(fd, EPOLLIN | EPOLLET);
}

void Epoll::modify(int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) < 0, "Epoll::modify");
}

void Epoll::modify(IOContext *c, uint32_t events)
{
    struct epoll_event ev;
    ev.data.ptr = (void *)c;
    ev.events = events;
    errif(epoll_ctl(epfd, EPOLL_CTL_MOD, c->getFd(), &ev) < 0, "Epoll::modify");
}

void Epoll::del(int fd)
{
    errif(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, 0) < 0, "Epoll::del");
}

int Epoll::waitEvents()
{
    // printf("epfd[%d] called waitEvents\n",epfd);
    int ret = epoll_wait(epfd, events.get(), MAX_EVENTS, -1);
    errif(ret < 0, "epoll_wait");
    return ret;
}

epoll_event *Epoll::getEvents()
{
    printf("epfd[%d] called getEvents\n", epfd);
    return events.get();
}

std::vector<IOContext *> Epoll::poll()
{
    std::vector<IOContext *> ret;
    int nfds = waitEvents();
    for (int i = 0; i < nfds; i++)
    {
        IOContext *c = (IOContext *)events[i].data.ptr;
        c->setRevents(events[i].events);
        ret.push_back(c);
    }
    return ret;
}