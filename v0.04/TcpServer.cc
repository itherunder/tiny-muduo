#include "TcpServer.h"

TcpServer::TcpServer()
    : epollFd_(0)
    , events_(INIT_EVENTS)
    , acceptor_(nullptr) {
    cout << "TcpServer ..." << endl;
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
    cout << "~TcpServer ..." << endl;
}

void TcpServer::NewConnection(int sockFd) {
    //每个new 都要记得回收
    TcpConnection* connection = new TcpConnection(epollFd_, sockFd);
    connections_[sockFd] = connection;
}

void TcpServer::Start() {
    epollFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd_ < 0)
        ERR_EXIT("epoll_create1");
    acceptor_ = new Acceptor(epollFd_);
    acceptor_->SetCallBack(this);
    acceptor_->Start();

    while (true) {
        int nready = epoll_wait(epollFd_, &*events_.begin(), static_cast<int>(events_.size()), -1);
        if (nready == -1)
            ERR_EXIT("epoll_wait");
        if (nready == 0)
            continue;
        if (nready == static_cast<int>(events_.size()))
            events_.resize(nready * 2);

        vector<Channel*> channels;//先把指针存起来，最后再一个一个调HandleEvent
        for (int i = 0; i < nready; ++i) {
            Channel* pChannel = static_cast<Channel*>(events_[i].data.ptr);
            pChannel->SetRevents(events_[i].events);
            channels.push_back(pChannel);
        }
        for (Channel* pChannel : channels) {
            pChannel->HandleEvent();
            if (pChannel->IsClosed()) {//处理完事件后，如果该fd 已经close 了就回收内存
                int fd = pChannel->GetSockFd();
                if (connections_.count(fd)) {
                    if (connections_[fd]) {
                        delete connections_[fd];
                        connections_[fd] = nullptr;
                    }
                    connections_.erase(fd);
                }
            }
        }
    }
}
