#include "Acceptor.h"

Acceptor::Acceptor(EventLoop* loop)
    : listenFd_(0)
    , idleFd_(0)
    , loop_(loop)
    , acceptChannel_(nullptr)
    , callBack_(nullptr) {
    cout << "[CONS] Acceptor ..." << endl;
}

Acceptor::~Acceptor() {
    if (acceptChannel_) {
        delete acceptChannel_;
        acceptChannel_ = nullptr;
    }
    cout << "[DECO] ~Acceptor ..." << endl;
}

void Acceptor::SetCallBack(IAcceptorCallBack* callBack) {
    callBack_ = callBack;
}

void Acceptor::Start() {
    listenFd_ = CreateAndListen();
    acceptChannel_ = new Channel(loop_, listenFd_);
    acceptChannel_->SetCallBack(this);
    acceptChannel_->EnableReading();
}

/*
* 继承自 IChannelCallBack，可以用用于处理 EPOLLIN
* 只需要在对应的 Channel 中将 this 指针存入其 callBack_
* 目前还只是一个 echo 服务器
*/
void Acceptor::OnIn(int sockFd) {//这个sockFd就是lisenFd，没有用
    cout << "[INFO] OnIn Listen From: " << sockFd << endl;
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
                ERR_EXIT("[ERRO] accept");
            close(connFd);
            idleFd_ = open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
        else
            ERR_EXIT("[ERRO] accept4");
    }

    callBack_->NewConnection(connFd);

    //连接成功
    cout << "[INFO] ip=" << inet_ntoa(cliAddr.sin_addr) << 
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
        ERR_EXIT("[ERRO] socket");
    
    sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(1116);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    if (setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("[ERRO] socket");

    if (bind(listenFd_, (sockaddr*)&srv_addr, sizeof(srv_addr)) < 0)
        ERR_EXIT("[ERRO] bind");
    
    if (listen(listenFd_, SOMAXCONN) < 0)
        ERR_EXIT("[ERRO] listen");

    Channel* pChannel = new Channel(loop_, listenFd_);
    pChannel->SetCallBack(this);
    pChannel->EnableReading();

    return listenFd_;
}
