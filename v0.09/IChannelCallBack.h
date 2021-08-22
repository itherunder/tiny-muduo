#ifndef _ICHANNEL_CALLBACK_H_
#define _ICHANNEL_CALLBACK_H_

class IChannelCallBack {
public:
    virtual void HandleRead() {}
    virtual void HandleWrite() {}
};

#endif //_ICHANNEL_CALLBACK_H_
