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
    Acceptor(EventLoop* pLoop);
    virtual ~Acceptor();
    void SetCallBack(IAcceptorCallBack* pCallBack);
    void Start();
    void HandleRead() override;
    void HandleWrite() override;

private:
    int CreateAndListen();

    int listenFd_;
    int idleFd_;
    EventLoop* pLoop_;
    Channel* pSocketAChannel_;
    IAcceptorCallBack* pCallBack_;
};

#endif //_ACCEPTOR_H_
