#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        return 1;
    }

    struct addrinfo hints;
    struct addrinfo *result, *rp;

    // 清空 hints 结构
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;     // 支持 IPv4 或 IPv6
    hints.ai_socktype = SOCK_STREAM; // 流式套接字

    // 调用 getaddrinfo() 函数
    int status = getaddrinfo(hostname, NULL, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }

    // 遍历结果链表，查找 IPv4 或 IPv6 地址
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;
            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, INET_ADDRSTRLEN);
            printf("IPv4 Address: %s\n", ipstr);
        } else if (rp->ai_family == AF_INET6) { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)rp->ai_addr;
            char ipstr[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipstr, INET6_ADDRSTRLEN);
            printf("IPv6 Address: %s\n", ipstr);
        }
    }

    freeaddrinfo(result);
    return 0;
}
