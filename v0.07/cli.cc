#include "Headers.h"

#define ERR_EXIT(m) \
        do { \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while (0)

int main() {
    int sock;
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");
    
    sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(1116);
    srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if (connect(sock, (sockaddr*)&srv_addr, sizeof(srv_addr)) < 0)
        ERR_EXIT("connect");
    
    //不设置成非阻塞的，太麻烦了
    // fcntl(sock, F_SETFL, SOCK_NONBLOCK);

    sockaddr_in local_addr;
    socklen_t addrlen = sizeof(local_addr);
    if (getsockname(sock, (sockaddr*)&local_addr, &addrlen) < 0)
        ERR_EXIT("getsockname");
    
    cout << "ip=" << inet_ntoa(local_addr.sin_addr) <<
        " port=" << ntohs(local_addr.sin_port) << endl;
    
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
        // cout << "[DEBUG] sendbuf: " << sendbuf;
        //这里如果EchoServer 没有返回内容会导致read 阻塞
        write(sock, sendbuf, strlen(sendbuf));
        read(sock, recvbuf, sizeof(recvbuf));
        
        // fputs(recvbuf, stdout);
        cout << "[RECV] " << recvbuf << endl;
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }
    return 0;
}