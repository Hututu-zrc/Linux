#pragma once
#include <iostream>
#include <memory>
#include <sys/epoll.h>
#include "Socket.hpp"
#include "Connection.hpp"
using namespace SocketModule;

class IOService : public Connection
{

private:
    int _sockfd;

public:
    IOService(int sockfd = -1)
        : _sockfd(sockfd)
    {
        SetSockfd(_sockfd);
        SetEvents(EPOLLIN | EPOLLET);
    }

    void Recv() override
    {
        LOG(LogLevel::DEBUG)<<"I am IOServive";
        char buff[1024];
        int n = recv(_sockfd, buff, sizeof(buff) - 1, 0);
        buff[n] = 0;
        std::cout << "client# " << buff << std::endl;
        // 数据处理后返回，这里暂时就是直接返回回显
        std::string message = "echo# ";
        message += buff;
        send(_sockfd, message.c_str(), message.size(), 0);
    }
    void Send() override
    {
    }
    void Excep() override
    {
    }
    void Who()
    {
        std::cout << "i am IOService" << std::endl;
    }

    ~IOService() {}
};