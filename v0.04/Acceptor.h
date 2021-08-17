#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "IChannelCallBack.h"
#include "IAcceptorCallBack.h"
#include "Channel.h"
#include "Define.h"

class Acceptor : public IChannelCallBack {
public:
    Acceptor(int epollFd);
    ~Acceptor();
    void SetCallBack(IChannelCallBack* callBack);
    void Start();
    virtual void OnIn(int sockFd);

private:
    int CreateAndListen();

    int epollFd_;
    int listenFd_;
    int idleFd_;
    Channel* acceptChannel_;
    IAcceptorCallBack* callBack_;
};

#endif //_ACCEPTOR_H_
