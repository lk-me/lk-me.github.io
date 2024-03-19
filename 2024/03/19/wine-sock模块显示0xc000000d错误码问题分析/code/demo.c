#include <stdio.h>
#include <winsock2.h>

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize winsock.\n");
        return 1;
    }

    // 创建套接字
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Failed to create socket.\n");
        WSACleanup();
        return 1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.1.1");  // 任意本地地址
    addr.sin_port = htons(0);  // 要绑定的端口号

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    // 设置远程主机地址
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(64320);  // 远程主机端口
    serverAddr.sin_addr.s_addr = inet_addr("192.0.131.249");  // 远程主机IP地址

    // 连接到远程主机
    if (connect(sockfd, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Failed to connect to the server.\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // 连接成功，进行其他操作...

    // 关闭套接字
    closesocket(sockfd);
    WSACleanup();

    return 0;
}