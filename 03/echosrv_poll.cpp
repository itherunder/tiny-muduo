#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <poll.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <vector>

using namespace std;

#define ERR_EXIT(m) \
        do { \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while (0)

typedef vector<pollfd> PollFdList;

int main() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);//防止孤儿进程

    //这里我们预备一个空闲的描述符用于处理EMFILE（使用fd过多的异常）
    // int idle_fd = open("/dev/null", O_RDONLY | O_CLOEXEC);
    int listen_fd = 0;//fcntl也可以设置socket

    if ((listen_fd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");
    
    sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(1116);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt");
    
    if (bind(listen_fd, (sockaddr*)&srv_addr, sizeof(srv_addr)) < 0)
        ERR_EXIT("bind");

    if (listen(listen_fd, SOMAXCONN) < 0)
        ERR_EXIT("listen");
    
    pollfd pfd;
    pfd.fd = listen_fd;
    pfd.events = POLLIN;

    PollFdList pollfds;
    pollfds.push_back(pfd);

    int nready;

    sockaddr_in peeraddr;
    socklen_t peerlen;
    int conn_fd;
    while (true) {
        nready = poll(&*pollfds.begin(), pollfds.size(), -1);
        if (nready == -1) {
            if (errno == EINTR)
                continue;
            ERR_EXIT("poll");
        }
        if (nready == 0)//啥也没有
            continue;
        
        if (pollfds[0].revents & POLLIN) {//监听到连接
            peerlen = sizeof(peeraddr);
            conn_fd = accept4(listen_fd, (sockaddr*)&peeraddr,
                        &peerlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
            if (conn_fd == -1)
                ERR_EXIT("accept4");

            // //这里如果发生异常并且异常为EMFILE时，就释放空闲的fd 进行accept 后再直接close
            // //便可以优雅地断开客户端的请求
            // if (conn_fd == -1) {
            //     if (errno == EMFILE) {
            //         close(idle_fd);
            //         if ((conn_fd = accept(listen_fd, NULL, NULL) < 0))
            //             ERR_EXIT("accept4");
            //         close(conn_fd);
            //         idle_fd = open("/dev/null", O_RDONLY | O_CLOEXEC);
            //         continue;
            //     }
            //     else
            //         ERR_EXIT("accept4");
            // }

            //加入到poll监听
            pfd.fd = conn_fd;
            pfd.events = POLLIN;
            pfd.revents = 0;
            pollfds.push_back(pfd);
            --nready;

            //连接成功
            cout << "ip=" << inet_ntoa(peeraddr.sin_addr) << 
                " port=" << ntohs(peeraddr.sin_port) << endl;
        }

        cout << "pollfds: " << pollfds.size() << endl;
        cout << "nready: " << nready << endl;
        //第一个是listen_fd
        for (auto it = pollfds.begin() + 1; it != pollfds.end() && nready > 0; ++it) {
            if (it->revents & POLLIN) {
                --nready;
                conn_fd = it->fd;
                char buf[1024] = {0};
                //非阻塞的socket
                int ret = read(conn_fd, buf, sizeof(buf));
                if (ret == -1)
                    ERR_EXIT("read");
                if (ret == 0) {
                    cout << "client close" << endl;
                    it = pollfds.erase(it);
                    --it;

                    close(conn_fd);
                    continue;
                }
                cout << "echo: " << buf << endl;
                write(conn_fd, buf, strlen(buf));
            }
        }
    }
    return 0;
}
