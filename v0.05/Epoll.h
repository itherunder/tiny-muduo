#ifndef _EPOLL_H_
#define _EPOLL_H_

class Epoll
{
public:
    Epoll();
    ~Epoll();

private:
    int epollFd_;
};

#endif //_EPOLL_H_
