#include "Channel.h"

#include <sys/epoll.h>

#include <iostream>

using namespace std;

Channel::Channel(int epollFd, int sockFd)
    : epollFd_(epollFd)
    , sockFd_(sockFd)
    , events_(0)
    , revents_(0)
    , callBack_(nullptr) {
    cout << "Channel ..." << endl;
}

Channel::~Channel() {
    Close();//从epoll中注销
    cout << "~Channel ..." << endl;
}

void Channel::SetCallBack(IChannelCallBack* callBack) {
    callBack_ = callBack;
}

void Channel::HandleEvent() {
    if (revents_ & EPOLLIN)
        callBack_->OnIn(sockFd_);
}

void Channel::SetRevents(int revents) {
    revents_ = revents;
}

int Channel::GetSockFd() {
    return sockFd_;
}

void Channel::EnableReading() {
    events_ |= EPOLLIN;//默认还是边沿触发的
    Update();
}

void Channel::Close() {
    //2.6.9版本后最后的 struct epoll_event & 参数可以为空指针
    epoll_ctl(epollFd_, EPOLL_CTL_DEL, sockFd_, nullptr);
}

void Channel::Update() {
    epoll_event event;
    event.data.ptr = this;
    event.events = events_;
    epoll_ctl(epollFd_, EPOLL_CTL_ADD, sockFd_, &event);
}
