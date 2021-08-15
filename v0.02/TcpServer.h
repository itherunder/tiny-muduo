#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/epoll.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

#define MAX_EVENTS 500

#define ERR_EXIT(m) \
        do { \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while(0)

typedef vector<epoll_event> EventList;

class TcpServer {
public:
    TcpServer();
    ~TcpServer();
    void Start();
};

#endif //_TCP_SERVER_H_