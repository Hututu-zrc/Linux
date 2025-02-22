

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <cstring>
#include <memory>

#include "UdpClient.hpp"
#include "Log.hpp"

using std::cerr;
using std::cout;
using std::endl;
using namespace LogModule;

#define CONV(type) (struct sockaddr *)(type)

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "argument less " << endl;
        return 1;
    }

    std::string ip = argv[1];
    int port = std::stoi(argv[2]);

    // 1、创建socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0)
    {
        LOG(LogLevel::FATAL) << "socket_fd";
        exit(1);
    }

    // 2、填充struct sockaddr信息
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = ::ntohs(port);
    server.sin_addr.s_addr = inet_addr(ip.c_str());

    // 3、client主体收发程序
    while (true)
    {
        // 3.1用户输入
        std::string input;
        cout << "Please# ";
        std::getline(std::cin, input);

        // 3.2 sendto函数
        // 客户端不需要bind函数 操作系统会自动为客户端分配一个可用的本地地址和端口
        int n = ::sendto(socket_fd, input.c_str(), input.size(), 0, CONV(&server), sizeof(server));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << strerror(errno);
            exit(1);
        }

        // 3.3 recvfrom函数
        char buff[1024];
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);

        int m = ::recvfrom(socket_fd, buff, sizeof(buff), 0, CONV(&temp), &len);
        if (m < 0)
        {
            LOG(LogLevel::FATAL) << strerror(errno);
            exit(2);
        }
        buff[m]=0;
        cout<<buff<<endl;
    }
    return 0;
}