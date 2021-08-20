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

void EchoServer::OnConnection(TcpConnection* connection) {
    cout << "INFO: Connection" << endl;
}

void EchoServer::OnMessage(TcpConnection* connection, const string& data) {
    cout << "INFO: Message" << endl;
    connection->Send(data);
}
