#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "IChannelCallBack.h"

class Channel {
public:
    Channel(int epollFd, int sockFd);
    ~Channel();
    void SetCallBack(IChannelCallBack* callBack);
    void HandleEvent();
    void SetRevents(int revents);
    int GetSockFd();
    void EnableReading();
    void Close();
    bool IsClosed();

private:
    void Update();
    int epollFd_;
    int sockFd_;
    int events_;
    int revents_;
    bool closed_;
    IChannelCallBack* callBack_;
};

#endif //_CHANNEL_H_
