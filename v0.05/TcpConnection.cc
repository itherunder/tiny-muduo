#include "TcpConnection.h"
#include "Headers.h"
#include "Define.h"

TcpConnection::TcpConnection(int epollFd, int sockFd)
    : epollFd_(epollFd)
    , sockFd_(sockFd)
    , channel_(nullptr) {
    channel_ = new Channel(epollFd_, sockFd);
    channel_->SetCallBack(this);
    channel_->EnableReading();
    cout << "TcpConnection ..." << endl;
}

TcpConnection::~TcpConnection() {
    if (channel_) {
        delete channel_;
        channel_ = nullptr;
    }
    cout << "~TcpConnection ..." << endl;
}

void TcpConnection::OnIn(int sockFd) {
    cout << "OnIn: " << sockFd << endl;
    if (sockFd < 0)
        ERR_EXIT("EPOLLIN sockFd < 0 error");
    char buf[MAX_BUFFER] = {0};
    int ret = read(sockFd, buf, sizeof(buf));
    if (ret == -1)
        ERR_EXIT("read");
    if (ret == 0) {
        cout << "client closed" << endl;
        close(sockFd);//记得关闭
        channel_->Close();
        return;//记得返回
    }
    cout << buf << endl;
    ret = write(sockFd, buf, strlen(buf));
    if (ret == -1)
        ERR_EXIT("write");
    if (ret < static_cast<int>(strlen(buf)))
        ERR_EXIT("write not finish one time");
}
