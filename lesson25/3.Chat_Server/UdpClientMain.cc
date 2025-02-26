

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <cstring>
#include <memory>

#include "UdpClient.hpp"
#include "Common.hpp"
#include "Log.hpp"

using std::cerr;
using std::cout;
using std::endl;
using namespace LogModule;
using namespace MutexModule;
int socket_fd = -1;
Mutex mutex;
void *RevMessage(void *arg)
{
    LockGuard lockguard(mutex);
    while (true)
    {
        char buff[1024];
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);

        int m = ::recvfrom(socket_fd, buff, sizeof(buff), 0, CONV(&temp), &len);
        if (m < 0)
        {
            LOG(LogLevel::FATAL) << strerror(errno);
            Die(Err::RECVFROM_ERR);
        }
        buff[m] = 0;
        cout << buff << endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "argument less " << endl;
        Die(Err::USAGE_ERR);
    }

    std::string ip = argv[1];
    int port = std::stoi(argv[2]);

    // 1、创建socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0)
    {
        LOG(LogLevel::FATAL) << "socket_fd";
        Die(Err::SOCKET_ERR);
    }

    // 2、填充struct sockaddr信息
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = ::htons(port);
    server.sin_addr.s_addr = inet_addr(ip.c_str());

    // 创建子进程，这样可以边收边发
    std::cout<<"client start"<<std::endl;
    pthread_t tid;
    pthread_create(&tid, nullptr, RevMessage, nullptr);
  
    // 3、client主体收发程序
    while (true)
    {
        // 3.1用户输入
        std::string input;
        cout << "Please# ";
        std::getline(std::cin, input);

    
        // 3.2 sendto函数
        // 客户端不需要bind函数 操作系统会自动为客户端分配一个可用的本地地址和端口
        LockGuard lockguard(mutex);
        int n = ::sendto(socket_fd, input.c_str(), input.size(), 0, CONV(&server), sizeof(server));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << strerror(errno);
            Die(Err::SENDTO_ERR);
        }
        // 3.3 recvfrom函数
    }

    pthread_join(tid, nullptr);
    return 0;
}