#include <iostream>
#include <vector>
#ifdef _WIN32
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <fcntl.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

#define PORT 8080
#define BUFFER_SIZE 1024

// 设置socket为非阻塞模式
void setNonBlocking(SOCKET socket) {
    #ifdef _WIN32
        unsigned long mode = 1;
        ioctlsocket(socket, FIONBIO, &mode);
    #else
        int flags = fcntl(socket, F_GETFL, 0);
        fcntl(socket, F_SETFL, flags | O_NONBLOCK);
    #endif
}

int main() {
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed\n";
            return 1;
        }
    #endif

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(serverSocket);
        return 1;
    }

    // 设置服务器socket为非阻塞模式
    setNonBlocking(serverSocket);

    std::vector<SOCKET> clientSockets;
    std::cout << "Server is listening on port " << PORT << " (Non-blocking mode)\n";

    while (true) {
        // 尝试接受新的连接
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        
        if (clientSocket != INVALID_SOCKET) {
            setNonBlocking(clientSocket);
            clientSockets.push_back(clientSocket);
            std::cout << "New client connected. Total clients: " << clientSockets.size() << std::endl;
        }

        // 处理所有已连接客户端的数据
        for (auto it = clientSockets.begin(); it != clientSockets.end();) {
            char buffer[BUFFER_SIZE];
            int bytesReceived = recv(*it, buffer, BUFFER_SIZE - 1, 0);

            if (bytesReceived > 0) {
                // 收到数据
                buffer[bytesReceived] = '\0';
                std::cout << "Received from client: " << buffer << std::endl;

                // 发送响应
                const char* response = "Message received";
                send(*it, response, strlen(response), 0);
                ++it;
            }
            else if (bytesReceived == 0 || 
                     (bytesReceived == SOCKET_ERROR && 
                      #ifdef _WIN32
                          WSAGetLastError() != WSAEWOULDBLOCK
                      #else
                          errno != EWOULDBLOCK && errno != EAGAIN
                      #endif
                     )) {
                // 客户端断开连接或发生错误
                std::cout << "Client disconnected\n";
                closesocket(*it);
                it = clientSockets.erase(it);
            }
            else {
                // 没有数据可读
                ++it;
            }
        }

        // 添加短暂延时，避免CPU占用过高
        #ifdef _WIN32
            Sleep(1);
        #else
            usleep(1000);
        #endif
    }

    // 关闭所有客户端socket
    for (SOCKET clientSocket : clientSockets) {
        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    
    #ifdef _WIN32
        WSACleanup();
    #endif

    return 0;
}