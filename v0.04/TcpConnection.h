#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include "IChannelCallBack.h"
#include "Channel.h"

class TcpConnection : public IChannelCallBack {
public:
    TcpConnection(/* args */);
    ~TcpConnection();
    virtual void OnIn(int sockFd);

private:
    int epollFd_;
    int sockFd_;
    Channel* channel_;
};


#endif //_TCP_CONNECTION_H_
