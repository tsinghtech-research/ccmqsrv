#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 创建服务器套接字
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // 准备服务器地址
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(5672);

    // 绑定套接字到地址和端口
    std::cout << connect(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) << std::endl;

    // 监听连接请求
    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Failed to listen." << std::endl;
        return 1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    // 接受客户端连接
    int clientSocket;
    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);
    if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength)) < 0) {
        std::cerr << "Failed to accept connection." << std::endl;
        return 1;
    }

    std::cout << "Client connected." << std::endl;

    // 向客户端发送消息
    const char *message = "Hello from server!";
    if (send(clientSocket, message, strlen(message), 0) < 0) {
        std::cerr << "Failed to send message." << std::endl;
        return 1;
    }

    // 关闭套接字
    close(clientSocket);
    close(serverSocket);

    return 0;
}
