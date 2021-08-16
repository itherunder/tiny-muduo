#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "IChannelCallBack"

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
#include <unordered_map>
#include <algorithm>

using namespace std;

#define MAX_EVENTS 500

#define ERR_EXIT(m) \
        do { \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while(0)

typedef vector<epoll_event> EventList;

class TcpServer : public IChannelCallBack {
public:
    TcpServer();
    ~TcpServer();
    void Start();
    virtual void OnIn(int sockFd);

private:
    void update(Channel* pChannel, int op);

    int epollFd_;
    int listenFd_;
    epoll_event events_[MAX_EVENTS];
    unordered_map<int, Channel*> channels_;
};

#endif //_TCP_SERVER_H_