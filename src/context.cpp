/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "context.hpp"
#include <iostream>

Context::Context(int cfd, Address address, Reactor *reactor,
                 std::function<void(Context *)> onConenct, std::function<void(int)> onDestroy) : __socket(std::make_unique<Socket>(cfd)),
                                                                                                 __buffer(std::make_unique<Buffer>()),
                                                                                                 __wbuffer(std::make_unique<Buffer>()),
                                                                                                 __reactor(reactor),
                                                                                                 __destroyContext(onDestroy),
                                                                                                 __onConn(std::move(onConenct)),
                                                                                                 __address(std::move(address))
{
    assert(__destroyContext);
    __ioContext = std::make_unique<IOContext>(__reactor, cfd);
    __ioContext->enableRead(); // register ioContext on reactor and therefore epoll
    __ioContext->enableWrite();
    __ioContext->setET();
    __ioContext->setReadCallback(std::bind(&Context::handleReadableEvent,this));
    __state = VALID;
    if(__onConn) __onConn(this);
}

Context::Context(Context&& other):  __ioContext(std::move(other.__ioContext)),
__state(other.__state)
{
    other.__state = MOVED;
}


Context::~Context() {}


void Context::flushWriteBuffer()
{
    assert(__state!=MOVED);
    int bytes_written;
    while (__wbuffer->size() > 0)
    {
        bytes_written = ::write(__socket_fd, __wbuffer->getReader(), __wbuffer->size());
        if (bytes_written > 0)
        {
            __wbuffer->moveReadCursor(bytes_written);
        }
        else if (bytes_written == -1 && (errno == EWOULDBLOCK || errno == EAGAIN))
        {
            break;
        }
        else if (bytes_written == 0)
        {
            __state = CLOSED;
            printf("read EOF, client fd %d disconnected\n", __socket_fd);
            this->destory();
            break;
        }
        else
        {
            __state = INVALID;
            printf("Unknown error on fd=%d, socket closed and context destroyed\n", __socket_fd);
            this->destory();
            break;
        }
    }
}

void Context::handleReadableEvent()
{
    assert(__state!=MOVED);
    int fd = __socket->getFd();
    int bytes_read;
    while (1)
    {
        printf("Enter loop\n");
        void *writer = __buffer->getWriter(Context::read_buf_size);
        bytes_read = ::read(fd, writer, Context::read_buf_size);
        if (bytes_read > 0)
        {
            if (Context::read_buf_size > bytes_read) 
            {
                __buffer->shrink(Context::read_buf_size - bytes_read);
            }
                
        }
        else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            __buffer->shrink(Context::read_buf_size);
            break;
        }
        else if (bytes_read == 0)
        {
            __buffer->shrink(Context::read_buf_size);
            __state = CLOSED;
            printf("read EOF, client fd %d disconnected\n", fd);
            this->destory();
            break;
        }
        else
        {
             __buffer->shrink(Context::read_buf_size);
            __state = INVALID;
            printf("Unknown error on fd=%d, socket closed and context destroyed\n", fd);
            this->destory();
            break;
        }
    }
    //printf("Context::handleReadableEvent(): moved readable data to bufer\n");
    if(__onRecv) __onRecv(this);
}

void Context::asyncWrite(const void *buf, size_t size)
{
    assert(__state!=MOVED);
    __wbuffer->put(buf, size);
    flushWriteBuffer();
}

ER Context::syncWrite(const void *buf, size_t size)
{
    assert(__state!=MOVED);
    int bytes_written;
    int remaining = size;
    printf("Context::syncWrite: size = %ld, __socket_fd = %d\n",size,__socket_fd);
    while (remaining > 0)
    {
        bytes_written = ::write(__socket_fd, buf, remaining);
        if (bytes_written > 0)
        {
            printf("Context::syncWrite: bytes_written = %d\n",bytes_written);
            remaining -= bytes_written;
        }
        else if (bytes_written == -1 && (errno == EWOULDBLOCK || errno == EAGAIN))
        {
            if (remaining > 0)
                asyncWrite((const char*)buf + (size - remaining), remaining);
            return ER::SUCCESS;
        }
        else if (bytes_written == 0)
        {
            __state = CLOSED;
            printf("read EOF, client fd %d disconnected\n", __socket_fd);
            this->destory();
            return ER::SOCKET_ERROR;
        }
        else
        {
            __state = INVALID;
            printf("Unknown error on fd=%d, socket closed and context destroyed\n", __socket_fd);
            this->destory();
            return ER::SOCKET_ERROR;
        }
    }
    return ER::SUCCESS;
}

ER Context::write(const std::string &data)
{
    assert(__state!=MOVED);
    return syncWrite(data.c_str(), data.size() + 1);
}

ER Context::write(std::string &&data)
{
    assert(__state!=MOVED);
    return write(std::move(data));
}

ER Context::write(const std::vector<char> &data)
{
    assert(__state!=MOVED);
    return syncWrite(data.data(), data.size());
}

ER Context::write(std::vector<char> &&data)
{
    assert(__state!=MOVED);
    return write(std::move(data));
}

ER Context::writeFile(std::string filePath)
{
    assert(__state!=MOVED);
    FILE *file;
    int bytes_read;
    if (file = ::fopen(filePath.c_str(), "r"))
    {
        while (1)
        {
            bytes_read = fread(__wbuffer->getWriter(Context::read_buf_size), sizeof(char), Context::read_buf_size, file);
            if (bytes_read < Context::read_buf_size)
            {
                __wbuffer->shrink(Context::read_buf_size - bytes_read);
            }
            if (bytes_read == 0)
            {
                break;
            }
        }
        flushWriteBuffer();
        return ER::SUCCESS;
    }
    else
    {
        return ER::FILE_ERROR;
    }
}

ER Context::writeFile(std::string filePath, size_t size)
{
    assert(__state!=MOVED);
    return ER::UNIMPLEMENTED;
}

const Address &Context::getAddress()
{
    assert(__state!=MOVED);
    return __address;
}

std::vector<char> Context::read()
{
    assert(__state!=MOVED);
    return std::move(__buffer->get());
}

std::string Context::readString()
{
    assert(__state!=MOVED);
    auto v = __buffer->getInnerVec();
    std::string ret(v.begin(), v.end());
    __buffer->clear();
    return ret;
}

Context::State Context::getState()
{
    assert(__state!=MOVED);
    return __state;
}

void Context::setOnRecv(std::function<void(Context *)> onRecv)
{
    assert(__state!=MOVED);
    __onRecv = std::move(onRecv);
}

void Context::destory()
{
    assert(__state!=MOVED);
    __destroyContext(__socket_fd);
}

// ~Context();
// std::vector<char> read();
// std::string readString();
// ER write(std::string);
// ER write(std::vector<char>);
// ER writeFile(std::string);
// ER writeFile(std::string, size_t);
// State getState();
// void setOnConn(std::function<void(Context*)>);
// void setOnRecv(std::function<void(Context*)>);
// void handleReadableEvent(); // passed to iocontext
// void destory();
