#include "EchoServer.h"

int main() {
    EventLoop loop;
    EchoServer echoServer(&loop);
    echoServer.Start();
    loop.Loop();
    return 0;
}
