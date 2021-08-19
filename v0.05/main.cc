#include "TcpServer.h"

int main() {
    EventLoop loop;
    TcpServer server(&loop);
    server.Start();
    loop.Loop();
    return 0;
}
