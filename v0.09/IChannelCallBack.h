#ifndef _ICHANNEL_CALLBACK_H_
#define _ICHANNEL_CALLBACK_H_

class IChannelCallBack {
public:
    virtual void HandleRead() = 0;
    virtual void HandleWrite() = 0;
};

#endif //_ICHANNEL_CALLBACK_H_
