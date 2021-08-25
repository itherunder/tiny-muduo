#ifndef _ECHO_SERVER_H
#define _ECHO_SERVER_H

#include "TcpServer.h"
#include "IMuduoUser.h"

class EchoServer
    : public IMuduoUser
    , public IRun {
public:
    EchoServer(EventLoop* loop);
    virtual ~EchoServer();
    void Start();
    void OnConnection(TcpConnection* connection) override;
    void OnMessage(TcpConnection* connection, Buffer& data) override;
    void OnWriteComplete(TcpConnection* connection) override;

    void Run(void* param) override;

private:
    EventLoop* loop_;
    TcpServer tcpServer_;
    int64_t timer_;
    int index_;
};

#endif //_ECHO_SERVER_H
