#include "Channel.h"

Channel::Channel(EventLoop* loop, int fd)
    : fd_(fd)
    , events_(0)
    , revents_(0)
    , closed_(false)
    , pLoop_(loop)
    , pCallBack_(nullptr) {
    // cout << "[CONS] Channel ..." << endl;
}

Channel::~Channel() {
    if (!closed_) Close();//从epoll中注销
    // cout << "[DECO] ~Channel ..." << endl;
}

void Channel::SetCallBack(IChannelCallBack* pCallBack) {
    pCallBack_ = pCallBack;
}

void Channel::HandleEvent() {
    if (revents_ & EPOLLIN)
        pCallBack_->HandleRead();
    if (revents_ & EPOLLOUT)
        pCallBack_->HandleWrite();
}

void Channel::SetRevents(int revents) {
    revents_ = revents;
}

int Channel::GetFd() {
    return fd_;
}

int Channel::GetEvents() {
    return events_;
}

void Channel::EnableReading() {
    events_ |= EPOLLIN;//默认还是边沿触发的
    Update(EPOLL_CTL_ADD);
}

void Channel::EnableWriting() {
    cout << "[DEBUG] Channel::EnableWriting" << endl;
    events_ |= EPOLLOUT;//默认还是边沿触发的
    Update(EPOLL_CTL_MOD);
}

void Channel::Close() {
    close(fd_);//记得关闭
    //2.6.9版本后最后的 struct epoll_event & 参数可以为空指针
    pLoop_->Update(this, EPOLL_CTL_DEL);
    closed_ = true;//表示该Channel 监听的fd 已经关闭
}

bool Channel::IsClosed() {
    return closed_;
}

void Channel::DisableWriting() {
    events_ |= ~EPOLLOUT;
    Update(EPOLL_CTL_MOD);
}

bool Channel::IsWriting() {
    return events_ & EPOLLOUT;
}

void Channel::Update(int op) {
    pLoop_->Update(this, op);
}
