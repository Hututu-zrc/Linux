

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

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

int socket_fd = -1;
struct sockaddr_in server;

void *RevMessage(void *arg)
{
    while (true)
    {

        // LockGuard lockguard(_mutex);
        //  这里加锁是错的，recvfrom一直等待消息，会卡住锁
        char buff[1024];
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);

        int m = ::recvfrom(socket_fd, buff, sizeof(buff), 0, CONV(&temp), &len);
        // LOG(LogLevel::DEBUG) << " RevMessage";

        if (m < 0)
        {
            LOG(LogLevel::FATAL) << strerror(errno);
            Die(Err::RECVFROM_ERR);
        }
        buff[m] = 0;
        cout << buff << endl;
    }
}

void Handler(int signo)
{

    std::string ExitMsg("QUIT");
    int n = ::sendto(socket_fd, ExitMsg.c_str(), ExitMsg.size(), 0, CONV(&server), sizeof(server));
    if (n < 0)
    {
        LOG(LogLevel::FATAL) << strerror(errno);
        Die(Err::SENDTO_ERR);
    }
    exit(0);
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "argument less " << endl;
        Die(Err::USAGE_ERR);
    }
    signal(2, Handler);

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

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = ::htons(port);
    server.sin_addr.s_addr = inet_addr(ip.c_str());

    // 创建子进程，这样可以边收边发
    // std::cout << "client start" << std::endl;
    pthread_t tid;
    pthread_create(&tid, nullptr, RevMessage, nullptr);

    // 第一次上线告诉所有的人
    {
        std::string StartMsg = "我上线了";

        int n = ::sendto(socket_fd, StartMsg.c_str(), StartMsg.size(), 0, CONV(&server), sizeof(server));
    }

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
            Die(Err::SENDTO_ERR);
        }
        // 3.3 recvfrom函数
    }

    pthread_join(tid, nullptr);
    return 0;
}