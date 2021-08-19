#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include "IChannelCallBack.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Headers.h"
#include "Define.h"

class TcpConnection : public IChannelCallBack {
public:
    TcpConnection(EventLoop* loop, int sockFd);
    virtual ~TcpConnection();
    virtual void OnIn(int sockFd);

private:
    int sockFd_;
    Channel* channel_;
};

#endif //_TCP_CONNECTION_H_
