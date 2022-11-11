#include <functional>
#include <vector>
#include "channel.h"

/*
 * Reactor wraps a group of tasks to be handled each
 * reactor is associated with one thread and one epoll;
 * the existence of reactor is for fully utilize a thread
 * to handle a group of I/O events (I/O multiplex +
 * multitthreading).
 */

class Reactor
{
private:
    Epoll ep;

public:
    Reactor();
    ~Reactor();
    void updateChan(Channel *);
    // add/modify channel; auto removed when channel is destroyed
    void run();
    // an infinite loop
};