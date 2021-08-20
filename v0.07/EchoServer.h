#ifndef _ECHO_SERVER_H
#define _ECHO_SERVER_H

#include "TcpServer.h"
#include "IMuduoUser.h"

class EchoServer : public IMuduoUser {
public:
    EchoServer(EventLoop* loop);
    virtual ~EchoServer();
    void Start();
    void OnConnection(TcpConnection* connection);
    void OnMessage(TcpConnection* connection, string* data);

private:
    EventLoop* loop_;
    TcpServer tcpServer_;
};

#endif //_ECHO_SERVER_H
