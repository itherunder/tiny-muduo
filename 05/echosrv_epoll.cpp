#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/epoll.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef vector<epoll_event> EventList;

#define ERR_EXIT(m) \
        do { \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while(0)

int main() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    int idle_fd = open("/dev/null", O_RDONLY | O_CLOEXEC);
    int listen_fd = 0;
    
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
    
    vector<int> clients;
    int epoll_fd = epoll_create1(EPOLL_CLOEXEC);

    epoll_event event;
    event.data.fd = listen_fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event);

    EventList events(16);
    sockaddr_in peer_addr;
    socklen_t peer_len;
    int conn_fd;

    int nready;
    while (true) {
        nready = epoll_wait(epoll_fd, &*events.begin(), events.size(), -1);
        if (nready == -1) {
            if (errno == EINTR)
                continue;
            ERR_EXIT("epoll_wait");
        }
        if (nready == 0)
            continue;
        if (nready == events.size())
            events.resize(nready * 2);

        for (int i = 0; i < nready; ++i) {
            if (events[i].data.fd == listen_fd) {
                peer_len = sizeof(peer_addr);
                conn_fd = accept4(listen_fd, (sockaddr*)&peer_addr,
                        &peer_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
                if (conn_fd == -1) {
                    if (errno == EMFILE) {
                        close(idle_fd);
                        if ((conn_fd = accept(listen_fd, NULL, NULL)))
                            ERR_EXIT("accept");
                        close(conn_fd);
                        idle_fd = open("/dev/null", O_RDONLY | O_CLOEXEC);
                        continue;
                    }
                    else
                        ERR_EXIT("accept4");
                }

                event.data.fd = conn_fd, 
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &event);

                //连接成功
                cout << "ip=" << inet_ntoa(peer_addr.sin_addr) << 
                    " port=" << ntohs(peer_addr.sin_port) << endl;
                continue;
            }
            conn_fd = events[i].data.fd;
            char buf[1024] = {0};
            int ret = read(conn_fd, buf, sizeof(buf));
            if (ret == -1)
                ERR_EXIT("read");
            if (ret == 0) {
                cout << "client closed" << endl;
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn_fd, &*(events.begin() + i));
                continue;
            }
            cout << buf << endl;
            write(conn_fd, buf, strlen(buf));
        }
    }
    return 0;
}