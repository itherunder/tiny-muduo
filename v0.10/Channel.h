#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "IChannelCallBack.h"
#include "EventLoop.h"
#include "Headers.h"
#include "Define.h"

class Channel {
public:
    Channel(EventLoop* pLoop, int sockFd);
    virtual ~Channel();
    void SetCallBack(IChannelCallBack* pCallBack);
    void HandleEvent();
    void SetRevents(int revents);
    int GetFd();
    int GetEvents();
    void EnableReading();
    void EnableWriting();
    void Close();
    bool IsClosed();
    void DisableWriting();
    bool IsWriting();

private:
    void Update(int op);
    int fd_;
    int events_;
    int revents_;
    bool closed_;
    EventLoop* pLoop_;
    IChannelCallBack* pCallBack_;
};

#endif //_CHANNEL_H_
