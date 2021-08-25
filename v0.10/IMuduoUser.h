#ifndef _IMUDUO_USER_H_
#define _IMUDUO_USER_H_

#include "TcpConnection.h"
#include "Define.h"
#include "Headers.h"

class IMuduoUser {
public:
    virtual void OnConnection(TcpConnection* pConn) = 0;
    virtual void OnMessage(TcpConnection* pConn, Buffer& data) = 0;
    virtual void OnWriteComplete(TcpConnection* pConn) = 0;
};

#endif //_IMUDUO_USER_H_
