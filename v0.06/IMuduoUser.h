#ifndef _IMUDUO_USER_H_
#define _IMUDUO_USER_H_

#include "TcpConnection.h"
#include "Define.h"
#include "Headers.h"

class IMuduoUser {
public:
    virtual void OnConnection(TcpConnection* connection) = 0;
    virtual void OnMessage(TcpConnection* connection, const string& data) = 0;
};

#endif //_IMUDUO_USER_H_
