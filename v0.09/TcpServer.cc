#include "TcpServer.h"

TcpServer::TcpServer(EventLoop* loop)
    : loop_(loop)
    , acceptor_(nullptr)
    , user_(nullptr) {
    //为了能在loop_中回收内存
    loop_->SetConnections(&connections_);
    cout << "[CONS] TcpServer ..." << endl;
}

TcpServer::~TcpServer() {
    //内存回收
    for (auto it = connections_.begin(); it != connections_.end(); ++it) {
        if (it->second) {
            delete it->second;
            it->second = nullptr;
        }
    }
    if (acceptor_) {
        delete acceptor_;
        acceptor_ = nullptr;
    }
    cout << "[DECO] ~TcpServer ..." << endl;
}

void TcpServer::NewConnection(int sockFd) {
    //每个new 都要记得回收
    TcpConnection* connection = new TcpConnection(loop_, sockFd);
    connections_[sockFd] = connection;
    connection->SetUser(user_);
    connection->Established();
    cout << "[INFO] TcpServer::NewConnection cur connection: " << connections_.size() << endl;
}

void TcpServer::Start() {
    acceptor_ = new Acceptor(loop_);
    acceptor_->SetCallBack(this);
    acceptor_->Start();
}

void TcpServer::SetCallback(IMuduoUser* user) {
    user_ = user;
}
