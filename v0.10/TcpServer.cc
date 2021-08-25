#include "TcpServer.h"

TcpServer::TcpServer(EventLoop* pLoop)
    : pLoop_(pLoop)
    , pAcceptor_(nullptr)
    , pUser_(nullptr) {
    //为了能在pLoop_中回收内存
    pLoop_->SetConns(&conns_);
    // cout << "[CONS] TcpServer ..." << endl;
}

TcpServer::~TcpServer() {
    //内存回收
    for (auto it = conns_.begin(); it != conns_.end(); ++it) {
        if (it->second) {
            delete it->second;
            it->second = nullptr;
        }
    }
    if (pAcceptor_) {
        delete pAcceptor_;
        pAcceptor_ = nullptr;
    }
    // cout << "[DECO] ~TcpServer ..." << endl;
}

void TcpServer::NewConnection(int sockFd) {
    //每个new 都要记得回收
    TcpConnection* pConn = new TcpConnection(pLoop_, sockFd);
    conns_[sockFd] = pConn;
    pConn->SetUser(pUser_);
    pConn->Established();
    cout << "[INFO] TcpServer::NewConnection cur pConn: " << conns_.size() << endl;
}

void TcpServer::Start() {
    pAcceptor_ = new Acceptor(pLoop_);
    pAcceptor_->SetCallBack(this);
    pAcceptor_->Start();
}

void TcpServer::SetCallback(IMuduoUser* pUser) {
    pUser_ = pUser;
}
