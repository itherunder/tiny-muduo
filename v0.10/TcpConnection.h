#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include "IChannelCallBack.h"
#include "IRun.h"
#include "EventLoop.h"
#include "Channel.h"
#include "IMuduoUser.h"
#include "Buffer.h"
#include "Headers.h"
#include "Define.h"

class TcpConnection : public IChannelCallBack
                    , public IRun {
public:
    TcpConnection(EventLoop* pLoop, int sockFd);
    virtual ~TcpConnection();
    void HandleRead() override;
    void HandleWrite() override;

    void Run(void* param) override;

    void SetUser(IMuduoUser* pUser);
    void Established();
    void Send(const string& message);

private:
    Channel* pSocketChannel_;
    IMuduoUser* pUser_;
    EventLoop* pLoop_;
    Buffer inBuf_;
    Buffer outBuf_;
};

#endif //_TCP_CONNECTION_H_
