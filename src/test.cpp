#include "server.hpp"
#include <iostream>

int main()
{
    Server s({"127.0.0.1", 18088});
    s.setOnConn([](Context *c)
                { std::cout << "New Client at " << c->getAddress().getHostString()
                            << ":" << c->getAddress().getPort() << std::endl; });
    s.setOnRecv(
        [](Context *c)
        { 
            std::cout << "Recv message by " << c->getAddress().getHostString()
                            << ":" << c->getAddress().getPort() << std::endl; 
            
            std::string msg = c->readString();
            std::cout<<msg<<" size of msg "<<msg.size()<<std::endl;
            c->write(msg);
        });
    s.serve();
}