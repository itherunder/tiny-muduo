## tiny-muduo
### v0.08
- 增加了Buffer 类用于封装 TcpConnection 中的inBuf_ 和outBuf_
- 增加了一个eventFd_ 在EventLoop 中，说是用来做异步的，但目前还不知道怎么个异步法
- 增加了一个IRun 类作为每次EchoServer 发送完数据给客户端后触发其实现的OnWriteComplete 函数的作用（具体就是在TcpConnection（现在也继承了IRun 类） 处理完成后将自己的指针放到EventLoop 类中新增的vector<IRun*> pendingFunctors_ 并且在每次Loop 后处理这些pendingFunctors_ 中的IRun*，详见EventLoop.cc 46 行，也确实是异步处理的，因为都是已经处理完成了才会把指针放到pendingFunctors_ 中