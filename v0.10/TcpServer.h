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
    TcpServer(EventLoop* pLoop);
    virtual ~TcpServer();
    void Start();
    virtual void NewConnection(int sockFd) override;
    void SetCallback(IMuduoUser* pUser);

private:
    unordered_map<int, TcpConnection*> conns_;
    EventLoop* pLoop_;
    Acceptor* pAcceptor_;
    IMuduoUser* pUser_;
};

#endif //_TCP_SERVER_H_
