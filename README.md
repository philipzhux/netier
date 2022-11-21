# Reactor Pattern C++ Concurrent TCP Server

The design follows the reactor patterns and encapsulate I/O multiplexed tcp socket read write handle logic into classes of Address, Socket, Epoll, ThreadPoll, IOContext, Context, Buffer, Reactor, and Acceptor.

TODO: More documentation

## Basic API

```C++
Server server({"127.0.0.1",8088});
server.setOnConn(
    [](Context *c) {
        std::cout<<"New Client at "<< c->getAddress().getAddressString() << std::endl;
    }
)
server.setOnRecv(
    [](Context *c) {
        std::cout<<"Receive message from "<< c->getAddress().getAddressString() << std::endl;
        c->sendFile(parseFileName(c->readString()));
    }
)
server.serve();

```
