#include "channel.h"
#include "buffer.h"
#include "socket.h"
class Connection {
private:
    Buffer _buffer;
public:
    Connection(Socket socket);
    ~Connection();

};