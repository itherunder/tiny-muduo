#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include "Epoll.h"
#include "TcpConnection.h"
#include "Headers.h"
#include "Define.h"

class EventLoop {
public:
    EventLoop();
    virtual ~EventLoop();
    void Loop();
    void Update(Channel* channel, int op);
    void SetConnections(unordered_map<int, TcpConnection*>* connections);

private:
    bool quit_;
    Epoll* poller_;
    unordered_map<int, TcpConnection*>* connections_;
};

#endif //_EVENT_LOOP_H_
