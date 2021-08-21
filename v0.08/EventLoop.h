#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include "Epoll.h"
#include "TcpConnection.h"
#include "Headers.h"
#include "Define.h"

class EventLoop : public IChannelCallBack {
public:
    EventLoop();
    virtual ~EventLoop();
    void Loop();
    void Update(Channel* channel, int op);
    void QueueLoop(IRun* run);
    void SetConnections(unordered_map<int, TcpConnection*>* connections);
    void HandleRead();
    void HandleWrite();

private:
    int CreateEventFd();
    void DoPendingFunctors();
    void WakeUp();
    bool quit_;
    int eventFd_;
    Channel* channel_;
    Epoll* poller_;
    unordered_map<int, TcpConnection*>* connections_;
    vector<IRun*> pendingFunctors_;
};

#endif //_EVENT_LOOP_H_
