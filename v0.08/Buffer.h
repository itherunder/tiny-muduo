#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "Headers.h"
#include "Define.h"

class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    const char* Peek();
    void Append(const string& str);
    void Retrieve(int len);
    string RetrieveAllAsString();
    string RetrieveAsString(int len);
    int ReadableBytes();
private:
    string buffer_;
};

#endif //_BUFFER_H_
