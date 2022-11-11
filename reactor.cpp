// class Reactor {
//     private:

//     public:
//     Reactor();
//     ~Reactor();
//     void updateChannel(Channel*);
//     //add/modify channel; auto removed when channel is destroyed
//     void run();
//     // a infinite loop

// };

#include "reactor.h"
#include <assert.h>
Reactor::Reactor()
{
    ep = Epoll();
}

Reactor::~Reactor()
{
}

void Reactor::updateChan(Channel *chan)
{
    ep.updateChan(chan);
}

void Reactor::run()
{
    while (true)
    {
       auto chans = ep.poll();
       for(auto& chan: chans) {
            assert(chan->isET());
            if(chan->isReadable()) chan->handleReadable();
            if(chan->isWritable()) chan->handleWritable();
       }
    }
}
