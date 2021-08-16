#include "TcpServer.h"

TcpServer::TcpServer()
    : epollFd_(0)
    , listenFd_(0)
    , idleFd_(0)
    , events_(INIT_EVENTS) {
    cout << "TcpServer ..." << endl;
}

TcpServer::~TcpServer() {
    cout << "~TcpServer ..." << endl;

}

/*
* 这里把创建了epollFd_, listenFd_ 并添加了listenFd_ 的Channel
*/
int TcpServer::CreateAndListen() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    idleFd_ = open("/dev/null", O_RDONLY | O_CLOEXEC);
    
    if ((listenFd_ = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");
    
    sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(1116);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    if (setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("socket");

    if (bind(listenFd_, (sockaddr*)&srv_addr, sizeof(srv_addr)) < 0)
        ERR_EXIT("bind");
    
    if (listen(listenFd_, SOMAXCONN) < 0)
        ERR_EXIT("listen");

    epollFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd_ < 0)
        ERR_EXIT("epoll_create1");

    Channel* pChannel = new Channel(epollFd_, listenFd_);
    pChannel->SetCallBack(this);
    pChannel->EnableReading();

    //在close 的时候可以回收对应的Channel
    channels_[listenFd_] = pChannel;

    return listenFd_;
}

/*
* 继承自 IChannelCallBack，可以用用于处理 EPOLLIN
* 只需要在对应的 Channel 中将 this 指针存入其 callBack_
*/
void TcpServer::OnIn(int sockFd) {
    cout << "OnIn: " << sockFd << endl;
    if (sockFd == listenFd_) {
        int connFd;
        sockaddr_in cliAddr;
        socklen_t cliLen;

        cliLen = sizeof(cliAddr);
        //设置成non-block io
        connFd = accept4(listenFd_, (sockaddr*)&cliAddr,
                &cliLen, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (connFd == -1) {
            if (errno == EMFILE) {
                close(idleFd_);
                if ((connFd = accept(listenFd_, NULL, NULL)))
                    ERR_EXIT("accept");
                close(connFd);
                idleFd_ = open("/dev/null", O_RDONLY | O_CLOEXEC);
            }
            else
                ERR_EXIT("accept4");
        }

        Channel* pChannel = new Channel(epollFd_, connFd);
        pChannel->SetCallBack(this);
        pChannel->EnableReading();
        
        channels_[connFd] = pChannel;

        //连接成功
        cout << "ip=" << inet_ntoa(cliAddr.sin_addr) << 
            " port=" << ntohs(cliAddr.sin_port) << endl;
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
    listenFd_ = CreateAndListen();
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
