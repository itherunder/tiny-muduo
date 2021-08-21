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

void EchoServer::OnMessage(TcpConnection* connection, Buffer& data) {
    //保证能够把所有内容发出去
    //不能发出去其实也在TcpConnection 对象的outBuf_ 中堆积
    while (data.ReadableBytes() != 0) {
        if (data.ReadableBytes() < MESSAGE_LENGTH) {
            connection->Send(data.RetrieveAllAsString());
            return;
        }
        string msg = data.RetrieveAsString(MESSAGE_LENGTH);
        connection->Send(msg);
    }
}

void EchoServer::OnWriteComplete(TcpConnection* connection) {
    cout << "EchoServer::OnWriteComplete write complete" << endl;
}
