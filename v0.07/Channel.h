#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "IChannelCallBack.h"
#include "EventLoop.h"
#include "Headers.h"
#include "Define.h"

class Channel {
public:
    Channel(EventLoop* loop, int sockFd);
    virtual ~Channel();
    void SetCallBack(IChannelCallBack* callBack);
    void HandleEvent();
    void SetRevents(int revents);
    int GetSockFd();
    int GetEvents();
    void EnableReading();
    void EnableWriting();
    void Close();
    bool IsClosed();
    void DisableWriting();
    bool IsWriting();

private:
    void Update(int op);
    int sockFd_;
    int events_;
    int revents_;
    bool closed_;
    EventLoop* loop_;
    IChannelCallBack* callBack_;
};

#endif //_CHANNEL_H_
