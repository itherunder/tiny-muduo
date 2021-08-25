#include "TcpConnection.h"

TcpConnection::TcpConnection(EventLoop* pLoop, int sockFd)
    : pSocketChannel_(nullptr)
    , pUser_(nullptr)
    , pLoop_(pLoop) {
    pSocketChannel_ = new Channel(pLoop, sockFd);
    pSocketChannel_->SetCallBack(this);
    pSocketChannel_->EnableReading();
    // cout << "[CONS] TcpConnection ..." << endl;
}

TcpConnection::~TcpConnection() {
    if (pSocketChannel_) {
        delete pSocketChannel_;
        pSocketChannel_ = nullptr;
    }
    // cout << "[DECO] ~TcpConnection ..." << endl;
}

//用户处理一下EPOLLIN 事件
void TcpConnection::HandleRead() {
    int sockFd = pSocketChannel_->GetFd();
    cout << "[INFO] TcpConnection::HandleRead sockFd: " << sockFd << endl;
    if (sockFd < 0)
        ERR_EXIT("[ERRO] TcpConnection::HandleRead EPOLLIN sockFd < 0 error");
    char buf[MAX_BUFFER] = {0};
    int ret = read(sockFd, buf, sizeof(buf));
    if (ret == -1)
        ERR_EXIT("[ERRO] TcpConnection::HandleRead read");
    if (ret == 0) {
        cout << "[INFO] TcpConnection::HandleRead client closed" << endl;
        pSocketChannel_->Close();
        return;//记得返回
    }
    cout << "[RECV] TcpConnection::HandleRead " << buf;
    inBuf_.Append(string(buf, ret));
    pUser_->OnMessage(this, inBuf_);
}

//用户处理一下EPOLLOUT 事件
void TcpConnection::HandleWrite() {
    int sockFd = pSocketChannel_->GetFd();
    if (pSocketChannel_->IsWriting()) {
        //把之前outBuf_ 中堆积的没有发送的数据发送了
        int n = write(sockFd, outBuf_.Peek(), outBuf_.ReadableBytes());
        if (n > 0) {
            cout << "[INFO] TcpConnection::HandleRead write " << n << " bytes data again" << endl;
            outBuf_.RetrieveAsString(n);
            //如果这次发送完了，就不再监听EPOLLOUT 事件
            //否则的话内核会一直触发EPOLLOUT 事件
            if (outBuf_.ReadableBytes() == 0) {
                pSocketChannel_->DisableWriting();
                pLoop_->QueueLoop(this, nullptr);
            }
        }
    }
}

void TcpConnection::Run(void* param) {
    pUser_->OnWriteComplete(this);
}

void TcpConnection::SetUser(IMuduoUser* pUser) {
    pUser_ = pUser;
}

void TcpConnection::Established() {
    if (pUser_)
        pUser_->OnConnection(this);
}

void TcpConnection::Send(const string& message) {
    // cout << "[DEBUG] TcpConnection::Send " << message << " ";
    int n = 0, sockFd = pSocketChannel_->GetFd();
    if (outBuf_.ReadableBytes() == 0) {
        n = write(sockFd, message.c_str(), message.length());
        if (n == -1)
            ERR_EXIT("[ERRO] TcpConnection::HandleRead write");
        if (n == static_cast<int>(message.length()))
            pLoop_->QueueLoop(this, nullptr);
    }
    if (n < static_cast<int>(message.length())) {
        //一次没有发完的话，存到outBuf_ 中并监听EPOLLOUT 事件
        outBuf_.Append(message.substr(n));
        if (!pSocketChannel_->IsWriting())//没有监听EPOLLOUT 则监听
            pSocketChannel_->EnableWriting();
    }
}
