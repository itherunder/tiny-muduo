#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "IAcceptorCallBack.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "IMuduoUser.h"
#include "Headers.h"
#include "Define.h"

using namespace std;

class TcpServer : public IAcceptorCallBack {
public:
    TcpServer(EventLoop* loop);
    virtual ~TcpServer();
    void Start();
    virtual void NewConnection(int sockFd) override;
    void SetCallback(IMuduoUser* user);

private:
    unordered_map<int, TcpConnection*> connections_;
    EventLoop* loop_;
    Acceptor* acceptor_;
    IMuduoUser* user_;
};

#endif //_TCP_SERVER_H_
