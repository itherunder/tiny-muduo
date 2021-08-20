#include "EventLoop.h"
#include "Channel.h"

EventLoop::EventLoop()
    : quit_(false)
    , poller_(new Epoll())
    , connections_(nullptr) {
    cout << "[CONS] EventLoop ..." << endl;
}

EventLoop::~EventLoop() {
    if (poller_) {
        delete poller_;
        poller_ = nullptr;
    }
    cout << "[DECO] ~EventLoop ..." << endl;
}

void EventLoop::Loop() {
    while (!quit_) {
        vector<Channel*> channels;//先把指针存起来，最后再一个一个调HandleEvent
        poller_->Poll(channels);
        for (Channel* pChannel : channels) {
            pChannel->HandleEvent();
            if (pChannel->IsClosed()) {//处理完事件后，如果该fd 已经close 了就回收内存
                int fd = pChannel->GetSockFd();
                if (connections_->count(fd)) {
                    if ((*connections_)[fd]) {
                        delete (*connections_)[fd];
                        (*connections_)[fd] = nullptr;
                    }
                    connections_->erase(fd);
                }
                cout << "[INFO] cur connection: " << connections_->size() << endl;
            }
        }
    }
}

void EventLoop::Update(Channel* channel, int op) {
    poller_->Update(channel, op);
}

void EventLoop::SetConnections(unordered_map<int, TcpConnection*>* connections) {
    connections_ = connections;
}
