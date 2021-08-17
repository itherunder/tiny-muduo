#include "Acceptor.h"
#include "Headers.h"

Acceptor::Acceptor(int epollFd)
    : epollFd_(epollFd) {
    cout << "Acceptor ..." << endl;
}

Acceptor::~Acceptor() {
    cout << "~Acceptor ..." << endl;
}

void Acceptor::SetCallBack(IChannelCallBack* callBack) {

}

void Acceptor::Start() {
    
}

/*
* 继承自 IChannelCallBack，可以用用于处理 EPOLLIN
* 只需要在对应的 Channel 中将 this 指针存入其 callBack_
* 目前还只是一个 echo 服务器
*/
void Acceptor::OnIn(int sockFd) {
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
    callBack_->NewConnection(connFd);

    //连接成功
    cout << "ip=" << inet_ntoa(cliAddr.sin_addr) << 
        " port=" << ntohs(cliAddr.sin_port) << endl;
}

/*
* 这里把创建了epollFd_, listenFd_ 并添加了listenFd_ 的Channel
*/
int Acceptor::CreateAndListen() {
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

    Channel* pChannel = new Channel(epollFd_, listenFd_);
    pChannel->SetCallBack(this);
    pChannel->EnableReading();

    return listenFd_;
}
