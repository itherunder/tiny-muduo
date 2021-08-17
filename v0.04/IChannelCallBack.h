#ifndef _ICHANNEL_CALLBACK_H_
#define _ICHANNEL_CALLBACK_H_

class IChannelCallBack {
public:
    virtual void OnIn(int sockFd) {}
};

#endif //_ICHANNEL_CALLBACK_H_
