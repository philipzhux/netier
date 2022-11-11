#include "channel.h"

Channel::Channel(Reactor* rt, int fd): rt(rt),fd(fd),registered(false) {}

Channel::~Channel() {
    if(fd>0) close(fd);
}

void Channel::enableRead(){
    setEvents(getEvents()|EPOLLIN);
}

void Channel::disableRead(){
    setEvents(getEvents()&~EPOLLIN);
}

void Channel::enableWrite(){
    setEvents(getEvents()|EPOLLOUT);
}

void Channel::disableRead(){
    setEvents(getEvents()&~EPOLLOUT);
}

void Channel::setET() {
    setEvents(getEvents()|EPOLLET);
}

void Channel::setET() {
    setEvents(getEvents()&~EPOLLET);
}

void Channel::setEvents(uint32_t ev){
    events = ev;
    rt->updateChan(this);
}

void Channel::setRevents(uint32_t ret){
    revents = ret;
}

bool Channel::isET() {
    return (getEvents()&EPOLLET)!=0;
}

bool Channel::isWritable() {
    return getRevents()&EPOLLOUT!=0;
}

bool Channel::isReadable() {
    return getRevents()&EPOLLIN!=0;
}

uint32_t Channel::getEvents(){
    return events;
}

uint32_t Channel::getRevents(){
    return revents;
}

int Channel::getFd() {
    return fd;
}
bool Channel::isRegistered() {
    return registered;
}

void Channel::setRegister() {
    registered = true;
}

void Channel::setReadCallback(std::function<void()> _cb) {
    readableCb = _cb;
}

void Channel::setWriteCallback(std::function<void()> _cb) {
    writableCb = _cb;
}


void Channel::handleWritable() {
    writableCb();
}

void Channel::handleReadable() {
    readableCb();
}