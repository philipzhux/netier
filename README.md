# Netier: Event-driven TCP Server Framework

The design follows the reactor patterns, RAII principles, and encapsulate I/O multiplexed tcp socket read write handle logic into classes of Address, Socket, Epoll, ThreadPoll, IOContext, Context, Buffer, Reactor, and Acceptor.

TODO: More documentation

As a side note of the design, unique_ptrs are held by the owner and other users get the raw pointers. However, it is guranteed that the life cycle of the raw pointer user is governed by the unique_ptr owner to avoid memory leak (slightly similar to some kind of GC). Such design tries to avoid the complexity of shared_ptr.

## Basic API

```C++
netier::Server server({"127.0.0.1",8088});
server.setOnConn(
    [](netier::Context *c) {
        std::cout<<"New Client at "<< c->getAddress().getAddressString() << std::endl;
    }
)
server.setOnRecv(
    [](netier::Context *c) {
        std::cout<<"Receive message from "<< c->getAddress().getAddressString() << std::endl;
        c->sendFile(parseFileName(c->readString()));
    }
)
server.serve();

```
