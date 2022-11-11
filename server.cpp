#include "server.h"

Epoll ep = Epoll();;

int main(int argc, char* argv[]) {
    Socket server = Socket(false);
    InetAddress serverAddr = InetAddress("0.0.0.0",22430);
    server.bind(serverAddr);
    server.listen();
    ep.add(server);

    
    // epfd = epoll_create1(0);
    // int fd = socket(AF_INET,SOCK_STREAM,0);
    // fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
    // struct sockaddr_in myAddr;
    // struct epoll_event events[MAX_EVENTS], ev;
    // memset(&ev,0,sizeof(ev));
    // memset(&events,0,sizeof(events));
    // ev.data.fd = fd;
    // ev.events = EPOLLIN | EPOLLET;
    // memset(&myAddr,0,sizeof myAddr);
    // myAddr.sin_family = AF_INET;
    // myAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    // myAddr.sin_port = htons(20251);
    // errif(bind(fd,(sockaddr*)&myAddr,sizeof(myAddr))!=0,"bind error");
    // errif(listen(fd,SOMAXCONN)!=0,"listen error");
    // epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
    // printf("added fd = %d; original = %d\n",ev.data.fd,fd);
    int nfds;
    while(true){
        nfds = ep.waitEvents();
        epoll_event* events = ep.getEvents();
        errif(nfds==-1,"epoll_wait");
        for(int i=0; i<nfds;i++) {
            if(events[i].data.fd==server.getFd()) {
                handleAccept(server);
            } else if(events[i].events & EPOLLIN) {
                handleRequest({events[i].data.fd});
            }
        }
    }
    // while(true) {
    //     struct sockaddr_in clientAddr;
    //     socklen_t clientAddrLen;
    //     memset(&clientAddr,0,sizeof clientAddr);
    //     int cfd = accept(fd,(sockaddr*)&clientAddr,&clientAddrLen);
    //     errif(cfd<0,"accept error.");
    //     printf("new client fd %d! IP: %s Port: %d\n", cfd, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        
        
        
    // }
}


void handleAccept(Socket& server) {
        InetAddress clientAddr;
        int cfd = server.accept(clientAddr);
        errif(cfd<0,"accept error.");
        printf("new client fd %d! IP: %s Port: %d\n", cfd, clientAddr.getIPString().c_str(), clientAddr.getPort());
        ep.add(cfd);
}

void handleRequest(Socket clientSocket) {
    clientSocket.setBlocking(false);
    char buffer[READ_BUFFER];
    memset(buffer,0,READ_BUFFER*sizeof(char));
    int readBytes;
    while(true){
        readBytes = read(clientSocket.getFd(),buffer,READ_BUFFER);
        if(readBytes>0){
            printf("[MSG] %s",buffer);
            write(clientSocket.getFd(),buffer,readBytes);
        }
        else if(readBytes==0){
            printf("[MSG] %s\n","EOF reached.");
            break;
        }
        else {
            if(errno==EAGAIN||errno==EWOULDBLOCK) break;
            if(errno==EINTR) continue;
            errif(true,"socket read error");
        }
    }
}