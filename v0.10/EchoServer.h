#ifndef _ECHO_SERVER_H
#define _ECHO_SERVER_H

#include "TcpServer.h"
#include "IMuduoUser.h"

class EchoServer
    : public IMuduoUser
    , public IRun {
public:
    EchoServer(EventLoop* pLoop);
    virtual ~EchoServer();
    void Start();
    void OnConnection(TcpConnection* pConn) override;
    void OnMessage(TcpConnection* pConn, Buffer& data) override;
    void OnWriteComplete(TcpConnection* pConn) override;

    void Run(void* param) override;

private:
    EventLoop* pLoop_;
    TcpServer tcpServer_;
    long timerId_;
    int index_;
};

#endif //_ECHO_SERVER_H
