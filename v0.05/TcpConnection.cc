#include "TcpConnection.h"

TcpConnection::TcpConnection(EventLoop* loop, int sockFd)
    : sockFd_(sockFd)
    , channel_(nullptr) {
    channel_ = new Channel(loop, sockFd);
    channel_->SetCallBack(this);
    channel_->EnableReading();
    cout << "[CONS] TcpConnection ..." << endl;
}

TcpConnection::~TcpConnection() {
    if (channel_) {
        delete channel_;
        channel_ = nullptr;
    }
    cout << "[DECO] ~TcpConnection ..." << endl;
}

void TcpConnection::OnIn(int sockFd) {
    cout << "[INFO] OnIn: " << sockFd << endl;
    if (sockFd < 0)
        ERR_EXIT("[ERRO] EPOLLIN sockFd < 0 error");
    char buf[MAX_BUFFER] = {0};
    int ret = read(sockFd, buf, sizeof(buf));
    if (ret == -1)
        ERR_EXIT("[ERRO] read");
    if (ret == 0) {
        cout << "[INFO] client closed" << endl;
        close(sockFd);//记得关闭
        channel_->Close();
        return;//记得返回
    }
    cout << buf << endl;
    ret = write(sockFd, buf, strlen(buf));
    if (ret == -1)
        ERR_EXIT("[ERRO] write");
    if (ret < static_cast<int>(strlen(buf)))
        ERR_EXIT("[ERRO] write not finish one time");
}
