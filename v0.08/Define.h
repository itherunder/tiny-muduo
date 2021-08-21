
class Acceptor;
class Channel;
class Epoll;
class EventLoop;
class IAcceptorCallBack;
class IChannelCallBack;
class TcpConnection;
class TcpServer;
class EchoServer;
class Buffer;

#define MAX_EVENTS 500
#define INIT_EVENTS 16
#define MAX_BUFFER 1024
#define MESSAGE_LENGTH 8

#define ERR_EXIT(m) \
        do { \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while(0)
