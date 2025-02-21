#include <iostream>
#include <string>
#include <thread> // 提供 std::this_thread
#include <chrono> // 提供 std::chrono
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif

const int PORT = 12345;       // 假设服务器端口为12345
const int BUFFER_SIZE = 1024; // 缓冲区大小为1024字节

void setNonBlocking(SOCKET socket)
{
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(socket, FIONBIO, &mode);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
#endif
}

int main()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
#endif

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (serverAddr.sin_addr.s_addr == INADDR_NONE)
    {
        std::cerr << "Invalid IP address\n";
        closesocket(clientSocket);
        return 1;
    }

    setNonBlocking(clientSocket);

    // 非阻塞连接
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
#ifdef _WIN32
        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            std::cerr << "Connection failed\n";
            closesocket(clientSocket);
            return 1;
        }
#else
        if (errno != EINPROGRESS)
        {
            std::cerr << "Connection failed\n";
            close(clientSocket);
            return 1;
        }
#endif
    }

    std::cout << "Connecting to server...\n";

    bool connected = false;
    bool messageInProgress = false;
    std::string currentMessage;

    while (true)
    {
        if (!connected)
        {
            // 检查连接是否建立
            fd_set writeSet;
            FD_ZERO(&writeSet);
            FD_SET(clientSocket, &writeSet);

            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000; // 100ms

            if (select(clientSocket + 1, NULL, &writeSet, NULL, &timeout) > 0)
            {
                connected = true;
                std::cout << "Connected to server\n";
            }
            continue;
        }

        if (!messageInProgress)
        {
            std::cout << "Enter message (or 'quit' to exit): ";
            messageInProgress = true;
        }

// 非阻塞方式检查用户输入
#ifdef _WIN32
        if (_kbhit())
        {
            char c = getchar();
#else
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0)
        {
#endif
            if (c == '\n')
            {
                if (currentMessage == "quit")
                {
                    break;
                }

                // 发送消息
                if (send(clientSocket, currentMessage.c_str(), currentMessage.length(), 0) == SOCKET_ERROR)
                {
#ifdef _WIN32
                    if (WSAGetLastError() != WSAEWOULDBLOCK)
                    {
                        std::cerr << "Send failed\n";
                        break;
                    }
#else
                    if (errno != EWOULDBLOCK && errno != EAGAIN)
                    {
                        std::cerr << "Send failed\n";
                        break;
                    }
#endif
                }

                currentMessage.clear();
                messageInProgress = false;
            }
            else
            {
                currentMessage += c;
            }
        }

        // 非阻塞接收服务器响应
        char buffer[BUFFER_SIZE];
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0';
            std::cout << "\nServer response: " << buffer << std::endl;
        }
        else if (bytesReceived == 0)
        {
            std::cout << "\nServer disconnected\n";
            break;
        }
        else
        {
#ifdef _WIN32
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
                std::cerr << "\nReceive failed\n";
                break;
            }
#else
            if (errno != EWOULDBLOCK && errno != EAGAIN)
            {
                std::cerr << "\nReceive failed\n";
                break;
            }
#endif
        }

        // 添加短暂延时
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

#ifdef _WIN32
    closesocket(clientSocket);
    WSACleanup();
#else
    close(clientSocket);
#endif

    return 0;
}