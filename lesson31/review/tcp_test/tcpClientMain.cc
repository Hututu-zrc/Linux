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
    if (argc != 3)
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
    sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = ::htons(std::stoi(args[2]));
    inet_pton(AF_INET, args[1], &client_addr.sin_addr);
    std::cout << "args: " << args[0] << " " << args[1] << " " << args[2] << std::endl;
    int n = connect(sockfd, (sockaddr *)(&client_addr), sizeof(client_addr));
    if (n < 0)
    {
        std::cerr << "connect err" << std::endl;
        return -1;
    }
    while (true)
    {
        std::string str;
        std::cout << "please# ";
        std::getline(std::cin, str);
        std::cout << "str: " << str << std::endl;
        socklen_t client_addr_len = sizeof(client_addr);
        n = ::send(sockfd, str.c_str(), str.size(), 0);
        if (n <= 0)
        {
            if (n == 0)
            {
                std::cout << "Server disconnected" << std::endl;
            }
            else
            {
                std::cerr << "recv error: " << strerror(errno) << std::endl;
            }
            break;
        }
        else
        {
            std::cout << "sendto success" << std::endl;
        }
        char buff[1024 * 4] = {0};
        sockaddr_in server_addr;
        socklen_t server_addr_len = sizeof(server_addr);
        memset(&server_addr, 0, sizeof(server_addr));
        n = ::recv(sockfd, buff, sizeof(buff) / sizeof(char) - 1, 0);
        if (n < 0)
        {
            std::cerr << "recvfrom err" << std::endl;
            return -1;
        }
        buff[n] = '\0';
        std::cout << "recv data: " << buff << std::endl;
    }
    ::close(sockfd);
    return 0;
}