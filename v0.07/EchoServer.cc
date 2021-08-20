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

void EchoServer::OnMessage(TcpConnection* connection, string* data) {
    cout << "INFO: Message" << endl;
    // //保证能够把所有内容发出去
    // while (!data->empty()) {
    //     if (data->length() < MESSAGE_LENGTH) {
    //         connection->Send(*data);
    //         *data = data->substr(data->length());
    //     }
    //     else {
    //         string msg = data->substr(0, MESSAGE_LENGTH);
    //         connection->Send(msg);
    //         *data = data->substr(MESSAGE_LENGTH);
    //     }
    // }
    while (data->length() > MESSAGE_LENGTH) {
        string msg = data->substr(0, MESSAGE_LENGTH);
        *data = data->substr(MESSAGE_LENGTH);
        connection->Send(msg);
    }
}
