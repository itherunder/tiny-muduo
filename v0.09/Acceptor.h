#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "IChannelCallBack.h"
#include "IAcceptorCallBack.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Headers.h"
#include "Define.h"

class Acceptor : public IChannelCallBack {
public:
    Acceptor(EventLoop* loop);
    virtual ~Acceptor();
    void SetCallBack(IAcceptorCallBack* callBack);
    void Start();
    void HandleRead() override;
    void HandleWrite() override;

private:
    int CreateAndListen();

    int listenFd_;
    int idleFd_;
    EventLoop* loop_;
    Channel* acceptChannel_;
    IAcceptorCallBack* callBack_;
};

#endif //_ACCEPTOR_H_
