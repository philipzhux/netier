#include "epoll.h"

Epoll::Epoll() {
    epfd = epoll_create1(0);
    events = new struct epoll_event[MAX_EVENTS];
    printf("epoll with epfd  = %d initialized.\n",epfd);
}

Epoll::~Epoll() {
    close(epfd);
    delete[] events;
    printf("epoll with epfd  = %d destroyed.\n",epfd);
}

void Epoll::updateChan(Channel* chan) {
    if(chan->isRegistered()) {
        add(chan,chan->getEvents());
        chan->setRegister();
    } else {
        modify(chan,chan->getEvents());
    }
}

void Epoll::add(int fd,uint32_t events) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    errif(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)<0,"Epoll::add");
}

void Epoll::add(Channel* chan,uint32_t events) {
    struct epoll_event ev;
    ev.data.ptr = (void*)chan;
    ev.events = events;
    errif(epoll_ctl(epfd,EPOLL_CTL_ADD,chan->getFd(),&ev)<0,"Epoll::add");
}

void Epoll::add(Socket const& socket,uint32_t events) {
    struct epoll_event ev;
    ev.data.fd = socket.getFd();
    ev.events = events;
    errif(epoll_ctl(epfd,EPOLL_CTL_ADD,socket.getFd(),&ev)<0,"Epoll::add");
    printf("epfd[%d] monitoring socket[%d]\n",epfd,socket.getFd());
}

void Epoll::add(Socket const& socket) {
    add(socket,EPOLLIN|EPOLLET);
}

void Epoll::add(int fd) {
    add(fd,EPOLLIN|EPOLLET);
}

void Epoll::modify(int fd,uint32_t events) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    errif(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev)<0,"Epoll::modify");
}

void Epoll::modify(Channel* chan,uint32_t events) {
    struct epoll_event ev;
    ev.data.ptr = (void*)chan;
    ev.events = events;
    errif(epoll_ctl(epfd,EPOLL_CTL_MOD,chan->getFd(),&ev)<0,"Epoll::add");
}

void Epoll::del(int fd) {
    errif(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,0)<0,"Epoll::del");
}

int Epoll::waitEvents(){
    printf("epfd[%d] called waitEvents\n",epfd);
    int ret = epoll_wait(epfd,events,MAX_EVENTS,-1);
    errif(ret<0,"epoll_wait");
    return ret;
}

epoll_event* Epoll::getEvents() {
    printf("epfd[%d] called getEvents\n",epfd);
    return events;
}

std::vector<Channel*> Epoll::poll() {
    std::vector<Channel*> ret;
    int nfds = waitEvents();
    for(int i=0;i<nfds;i++){
        Channel* chan = (Channel*)events[i].data.ptr;
        chan->setRevents(events[i].events);
        ret.push_back(chan);
    }
    return ret;
}