#include "EchoServer.h"

EchoServer::EchoServer(EventLoop* loop)
    : loop_(loop)
    , tcpServer_(loop)
    , timer_(-1)
    , index_(0) {
    tcpServer_.SetCallback(this);
    // cout << "[CONS] EchoServer ..." << endl;
}

EchoServer::~EchoServer() {
    // cout << "[DECO] EchoServer ..." << endl;
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
            break;//已经处理完了，退出循环
        }
        string msg = data.RetrieveAsString(MESSAGE_LENGTH);
        connection->Send(msg);
    }
    // cout << "[DEBUG] EchoServer::OnMessage" << endl;
    timer_ = loop_->RunEvery(1.5, this);
    // timer_ = loop_->RunAfter(1.5, this);
}

void EchoServer::OnWriteComplete(TcpConnection* connection) {
    cout << "EchoServer::OnWriteComplete write complete" << endl;
}

void EchoServer::Run(void* param) {
    cout << "[CALL] callback from timer(" << param << "): " << index_ << endl;
    if (index_++ == 3) {
        loop_->CancelTimer(timer_);
        index_ = 0;
    }
}
