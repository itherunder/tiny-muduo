#include "TcpConnection.h"

TcpConnection::TcpConnection(EventLoop* loop, int sockFd)
    : sockFd_(sockFd)
    , channel_(nullptr)
    , user_(nullptr)
    , inBuf_(new string())
    , outBuf_(new string()) {
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
    if (inBuf_) {
        delete inBuf_;
        inBuf_ = nullptr;
    }
    if (outBuf_) {
        delete outBuf_;
        outBuf_ = nullptr;
    }
    cout << "[DECO] ~TcpConnection ..." << endl;
}

//用户处理一下EPOLLIN 事件
void TcpConnection::HandleRead() {
    int sockFd = channel_->GetSockFd();
    cout << "[INFO] HandleRead: " << sockFd << endl;
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
    cout << "[RECV] " << buf << endl;
    inBuf_->append(buf, ret);
    user_->OnMessage(this, inBuf_);
}

//用户处理一下EPOLLOUT 事件
void TcpConnection::HandleWrite() {
    int sockFd = channel_->GetSockFd();
    if (channel_->IsWriting()) {
        //把之前outBuf_ 中堆积的没有发送的数据发送了
        int n = write(sockFd, outBuf_->c_str(), outBuf_->length());
        if (n > 0) {
            cout << "[INFO] write " << n << " bytes data again" << endl;
            *outBuf_ = outBuf_->substr(n);
            //如果这次发送完了，就不再监听EPOLLOUT 事件
            //否则的话内核会一直触发EPOLLOUT 事件
            if (outBuf_->empty())
                channel_->DisableWriting();
        }
    }
}

void TcpConnection::SetUser(IMuduoUser* user) {
    user_ = user;
}

void TcpConnection::Established() {
    if (user_)
        user_->OnConnection(this);
}

void TcpConnection::Send(const string& message) {
    int n = 0;
    if (outBuf_->empty()) {
        n = write(sockFd_, message.c_str(), message.length());
        if (n == -1)
            ERR_EXIT("[ERRO] write");
    }
    if (n < static_cast<int>(message.length())) {
        //一次没有发完的话，存到outBuf_ 中并监听EPOLLOUT 事件
        outBuf_->append(message.substr(n));
        if (!channel_->IsWriting())//没有监听EPOLLOUT 则监听
            channel_->EnableWriting();
    }
}
