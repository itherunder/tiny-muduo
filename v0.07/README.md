## tiny-muduo
### v0.07
- TcpConnection 中加入了string* inBuf_ 以及string* outBuf_ 用作缓冲区，并且加入了EPOLLOUT 事件的监听（当TcpConnection 的Send 函数没有发送完所有的message 的话就会EnableWriting，即监听EPOLLOUT
- 把IChannelCallBack 类中的OnIn 函数删了，改成了HandleRead 和HandleWrite，Acceptor 类中的OnIn 也改成了HandleRead，sockFd 可以通过对象的channel_.GetSockFd() 获取
- 还把IMuduoUser 类中的OnMessage 的参数const string& 改成了 string* 用于TcpConnection 传里面的string* inBuf_，TcpConnection 在析构的时候delete 了inBuf_ 和outBuf_，目前为止还没有任何内存泄漏