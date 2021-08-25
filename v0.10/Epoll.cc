#include "Epoll.h"

Epoll::Epoll()
    : events_(MAX_EVENTS) {
    epollFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd_ < 0)
        ERR_EXIT("[ERRO] epoll_create1");
    // cout << "[CONS] Epoll ..." << endl;
}

Epoll::~Epoll() {
    // cout << "[DECO] ~Epoll ..." << endl;
}

void Epoll::Poll(vector<Channel*>& pChannels) {
    int nready = epoll_wait(epollFd_, &*events_.begin(), static_cast<int>(events_.size()), -1);
    cout << endl << "[DEBUG] Epoll::Poll nready: " << nready << endl;
    if (nready == -1)
        ERR_EXIT("[ERRO] Epoll::Poll epoll_wait");
    if (nready == static_cast<int>(events_.size()))
        events_.resize(nready * 2);
    for (int i = 0; i < nready; ++i) {
        Channel* pChannel = static_cast<Channel*>(events_[i].data.ptr);
        pChannel->SetRevents(events_[i].events);
        pChannels.push_back(pChannel);
    }
}

void Epoll::Update(Channel* pChannel, int op) {
    epoll_event event;
    event.data.ptr = pChannel;
    event.events = pChannel->GetEvents();
    int fd = pChannel->GetFd();
    epoll_ctl(epollFd_, op, fd, &event);
}
