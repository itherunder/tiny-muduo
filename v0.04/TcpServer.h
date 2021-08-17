#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "IAcceptorCallBack.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "Headers.h"

using namespace std;

typedef vector<epoll_event> EventList;

class TcpServer : public IAcceptorCallBack {
public:
    TcpServer();
    ~TcpServer();
    void Start();
    virtual void NewConnection(int sockFd);

private:
    void Update(Channel* pChannel, int op);

    int epollFd_;
    int idleFd_;
    // epoll_event events_[MAX_EVENTS];
    EventList events_;
    unordered_map<int, TcpConnection*> connections_;
    Acceptor* acceptor_;
};

#endif //_TCP_SERVER_H_
