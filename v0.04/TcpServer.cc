#include "TcpServer.h"

TcpServer::TcpServer()
    : epollFd_(0)
    , idleFd_(0)
    , events_(INIT_EVENTS) {
    cout << "TcpServer ..." << endl;
}

TcpServer::~TcpServer() {
    //内存回收
    for (auto it = connections_.begin(); it != connections_.end(); ++it)
        if (it->second) delete it->second;
    cout << "~TcpServer ..." << endl;
}

void TcpServer::OnIn(int sockFd) {
    cout << "OnIn: " << sockFd << endl;
    if (sockFd == listenFd_) {
        
    } else {
        if (sockFd < 0)
            ERR_EXIT("EPOLLIN sockFd < 0 error");
        char buf[MAX_BUFFER] = {0};
        int ret = read(sockFd, buf, sizeof(buf));
        if (ret == -1)
            ERR_EXIT("read");
        if (ret == 0) {
            cout << "client closed" << endl;
            Channel* pChannel = channels_[sockFd];
            pChannel->Close();//调用epoll_ctl 删除该事件
            close(sockFd);//记得关闭
            delete pChannel;//删除对应的Channel
            channels_.erase(sockFd);
            return;//记得返回
        }
        cout << buf << endl;
        ret = write(sockFd, buf, strlen(buf));
        if (ret == -1)
            ERR_EXIT("write");
        if (ret < static_cast<int>(strlen(buf)))
            ERR_EXIT("write not finish one time");
    }
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
        for (Channel* pChannel : channels)
            pChannel->HandleEvent();
    }
}
