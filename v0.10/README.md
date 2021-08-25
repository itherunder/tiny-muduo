## tiny-muduo
### v0.09
- 增加了时间戳Timestamp 定时器队列TimerQueue，队列类里面有Timer 类（用于存储运行时的时间stamp_、运行间隔interval_、运行回调对象run_等）。使用timerfd_create、timerfd_settime 对TimerQueue 中的timerFd_ 进行操作，其实就是利用ResetTimerFd 对操作系统的定时器设定超时时间懂吧
- 删除了TcpConnection 中的sockFd_（因为channel_ 里面就有sockFd_，就不冗余存储了，close(sockFd_) 也放到了Channel 的Close 函数里面了
- 在子类里面的父类函数声明中加上了override（以免出错
- 使用set<pair<Timestamp,Timer*>> 存储所有定时时间（有周期性时间和一次性事件，并且在timerFd_ 对应的channel_ 的回调函数HandleRead 中进行处理（一旦超时都会在这个timerFd_ 上面产生事件，所以弄一个channel_ 用epoll 监听timerFd_ 就可以了
- 当然目前还存在多线程使用共同的EchoServer 的问题，就比如现在EchoServer 里面的timer 回调函数Run 其实就共用了index_ 导致两个客户端在同时调用这个Run 函数的时候有一个客户端会不满足index_++ == 3 而无法CancelTimer，明天再说