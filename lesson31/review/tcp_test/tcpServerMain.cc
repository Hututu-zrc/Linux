#include <sys/socket.h> // 基本套接字定义和函数(socket、bind、sendto等)
#include <netinet/in.h> // Internet地址族相关定义(sockaddr_in结构体)
#include <arpa/inet.h>  // 地址转换函数(inet_pton、inet_ntop等)
#include <unistd.h>     // close()函数
#include <string.h>
#include <iostream>
#include <string>
#include <signal.h>

void Exit(int)
{
    std::cout << "server recv exit signal!" << std::endl;
    ::exit(0);
}

int main(int argc, char *args[])
{
    signal(2, Exit);
    if (argc != 2)
    {
        std::cerr << "参数错误，请重新输入！" << std::endl;
        return 1;
    }
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket绑定错误" << std::endl;
        return -1;
    }
    sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = ::htons(std::stoi(args[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    int n = ::bind(sockfd, (sockaddr *)(&server_addr), sizeof(server_addr));
    if (n < 0)
    {
        std::cerr << "bind绑定错误" << std::endl;
        return -1;
    }
    n = ::listen(sockfd, 5);
    if (n < 0)
    {
        std::cerr << "listen绑定错误" << std::endl;
        return -1;
    }
    // 这里是长连接通信，收到一个链接后就循环接收，直到服务器退出
    // 这里有缺陷，暂时只做展示
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t addr_len = sizeof(client_addr);

    int listenfd = accept(sockfd, (sockaddr *)(&client_addr), &addr_len);
    if (listenfd < 0)
    {
        std::cerr << "accept err" << std::endl;
        return -1;
    }
    while (true)
    {

        char buff[1024 * 4] = {0};
        memset(buff, 0, sizeof(buff) / sizeof(char));
        n = ::recv(listenfd, buff, sizeof(buff) / sizeof(char) - 1, 0);
        if (n < 0)
        {
            std::cerr << "recvfrom err" << std::endl;
            return -1;
        }
        else if (n == 0)
        {
            return 0;
        }
        buff[n] = '\0';
        std::cout << "recv data: " << buff << std::endl;
        std::string str = "echo# ";
        str += buff;
        n = ::send(listenfd, str.c_str(), str.size(), 0);
        if (n < 0)
        {
            std::cerr << "sento err" << std::endl;
            return -1;
        }
        else
        {
            std::cout << "sendto success" << std::endl;
        }
    }
    ::close(listenfd);
    ::close(sockfd);
    return 0;
}

// int main(int argc, char *args[])
// {
//     signal(2, Exit);
//     if (argc != 2)
//     {
//         std::cerr << "参数错误，请重新输入！" << std::endl;
//         return 1;
//     }
//     int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0)
//     {
//         std::cerr << "socket绑定错误" << std::endl;
//         return -1;
//     }
//     sockaddr_in server_addr, client_addr;
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = ::htons(std::stoi(args[1]));
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     int n = ::bind(sockfd, (sockaddr *)(&server_addr), sizeof(server_addr));
//     if (n < 0)
//     {
//         std::cerr << "bind绑定错误" << std::endl;
//         return -1;
//     }
//     n = ::listen(sockfd, 5);
//     if (n < 0)
//     {
//         std::cerr << "listen绑定错误" << std::endl;
//         return -1;
//     }
//     //这里是短连接通信，每次accept接受一个连接后，传递消息就关闭

//     while (true)
//     {

//         char buff[1024 * 4] = {0};
//         memset(&client_addr, 0, sizeof(client_addr));
//         socklen_t addr_len = sizeof(client_addr);

//         int listenfd = accept(sockfd, (sockaddr *)(&client_addr), &addr_len);
//         if (listenfd < 0)
//         {
//             std::cerr << "accept err" << std::endl;
//             return -1;
//         }

//         n = ::recv(listenfd, buff, sizeof(buff) / sizeof(char) - 1, 0);
//         if (n < 0)
//         {
//             std::cerr << "recvfrom err" << std::endl;
//             return -1;
//         }
//         buff[n] = '\0';
//         std::cout << "recv data: " << buff << std::endl;
//         std::string str = "echo# ";
//         str += buff;
//         n = ::send(listenfd, str.c_str(), str.size(), 0);
//         if (n < 0)
//         {
//             std::cerr << "sento err" << std::endl;
//             return -1;
//         }
//         else
//         {
//             std::cout << "sendto success" << std::endl;
//         }
//         ::close(listenfd);
//     }
//     ::close(sockfd);
//     return 0;
// }