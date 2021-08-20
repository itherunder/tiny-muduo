## tiny-muduo
### v0.06
- 增加了一个IMuduoUser 类，只要继承自这个类的同时需要含有TcpServer 以及EventLoop 对象就可以实现纯虚函数OnConnection 处理连接后的逻辑，OnMessage 处理收到消息后的逻辑（只传了TcpConnection* 和const string&，当然不太行
- 继承IMuduoUser 类实现了一个EchoServer 类，OnMessage 中进行echo 服务（而不是放到TcpConnection 直接处理了）
- 上面的实现则是通过在TcpServer、TcpConnection 中加入一个IMuduoUser对象 的user_ 指针作为callback 对象进行事件处理