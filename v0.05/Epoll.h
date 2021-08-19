#ifndef _EPOLL_H_
#define _EPOLL_H_

#include "Channel.h"
#include "Headers.h"
#include "Define.h"

class Epoll {
public:
    Epoll();
    virtual ~Epoll();
    void Poll(vector<Channel*>& channels);
    void Update(Channel* channel, int op);

private:
    int epollFd_;
    // epoll_event events_[MAX_EVENTS];
    EventList events_;
};

#endif //_EPOLL_H_
