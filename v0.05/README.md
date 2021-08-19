## tiny-muduo
### v0.05
- 添加了Epoll 和EventLoop 类，分别用于封装epollFd 和之前TcpServer 中的while 循环
- 重构了之前的Channel、Acceptor 并将其中的epollFd_ 替换成了EventLoop* loop_（包含了一个Epoll 的对象指针poller_
- 修改了Channel 类的Update 函数，直接调用Channel 中的loop_ 的Update 函数，而loop_ 的Update 函数又是直接调用的poller_ 的Update 函数，我加了一个参数Update(Channel* channel, int op)，这个op 就是用来指定具体是EPOLL_CTL_ADD 还是EPOLL_CTL_DEL 操作（因为在cli 断开连接后，对应的Channel 对象就能够知道，在回收内存的同时还应该从epoll 中del 该fd
- TcpServer 向EventLoop 传递了一个connections_ 的指针，因为EventLoop 的loop 函数里面可以感知到cli 的断开，所以需要把TcpServer 中的字典也传过去用于内存回收