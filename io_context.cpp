/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "io_context.h"

IOContext::IOContext(Reactor* rt, int fd): __reactor(rt),__fd(fd),__registered(false) {}

IOContext::~IOContext() {
    __reactor->delIOContext(this);
}

void IOContext::enableRead(){
    setEvents(getEvents()|EPOLLIN);
}

void IOContext::disableRead(){
    setEvents(getEvents()&~EPOLLIN);
}

void IOContext::enableWrite(){
    setEvents(getEvents()|EPOLLOUT);
}

void IOContext::disableRead(){
    setEvents(getEvents()&~EPOLLOUT);
}

void IOContext::setET() {
    setEvents(getEvents()|EPOLLET);
}

void IOContext::setLT() {
    setEvents(getEvents()&~EPOLLET);
}

void IOContext::setEvents(uint32_t ev){
    __events = ev;
    __reactor->addIOContext(this);
}

void IOContext::setRevents(uint32_t ret){
    __revents = ret;
}

bool IOContext::isET() {
    return (getEvents()&EPOLLET)!=0;
}

bool IOContext::isWritable() {
    return getRevents()&EPOLLOUT!=0;
}

bool IOContext::isReadable() {
    return getRevents()&EPOLLIN!=0;
}

uint32_t IOContext::getEvents(){
    return __events;
}

uint32_t IOContext::getRevents(){
    return __revents;
}

int IOContext::getFd() {
    return __fd;
}
bool IOContext::isRegistered() {
    return __registered;
}

void IOContext::setRegister() {
    __registered = true;
}

void IOContext::setReadCallback(std::function<void(void)> cb) {
    __readableCb = cb;
}

void IOContext::setWriteCallback(std::function<void(void)> cb) {
    __writableCb = cb;
}


void IOContext::handleWritable() {
    if(__writableCb) __writableCb();
}

void IOContext::handleReadable() {
    if(__readableCb) __readableCb();
}