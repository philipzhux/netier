#ifndef SERVER_H
#define SERVER_H
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "error.h"
#include "epoll.h"
#include "socket.h"
#include "addr.h"

#define READ_BUFFER 1024

typedef void handler(int);
void handleRequest(Socket socker);
void handleAccept(Socket& socker);
#endif