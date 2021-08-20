#include "EchoServer.h"

EchoServer::EchoServer(EventLoop* loop)
    : loop_(loop)
    , tcpServer_(loop) {
    tcpServer_.SetCallback(this);
    cout << "[CONS] EchoServer ..." << endl;
}

EchoServer::~EchoServer() {
    cout << "[DECO] EchoServer ..." << endl;
}

void EchoServer::Start() {
    tcpServer_.Start();
}

void EchoServer::OnConnection(TcpConnection* connection) {}

void EchoServer::OnMessage(TcpConnection* connection, string* data) {
    //保证能够把所有内容发出去
    //不能发出去其实也在TcpConnection 对象的outBuf_ 中堆积
    while (!data->empty()) {
        if (data->length() < MESSAGE_LENGTH) {
            connection->Send(*data);
            *data = data->substr(data->length());
            return;
        }
        string msg = data->substr(0, MESSAGE_LENGTH);
        connection->Send(msg);
        *data = data->substr(MESSAGE_LENGTH);
    }
}
