#ifndef _IACCEPTOR_CALLBACK_H_
#define _IACCEPTOR_CALLBACK_H_

class IAcceptorCallBack {
public:
    virtual void NewConnection(int sockFd) {}
};

#endif //_IACCEPTOR_CALLBACK_H_
