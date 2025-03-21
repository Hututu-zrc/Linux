#pragma once
#include <iostream>
#include <string>
#include <sys/select.h>
#include "Socket.hpp"

using namespace SocketModule;

class SelectServer
{
private:
    uint16_t _port;
    std::unique_ptr<Socket> _socket;
    bool _isrunning;

public:
    SelectServer(std::string port)
        : _port(stoi(port)),
          _socket(std::make_unique<TcpSocket>()),
          _isrunning(false)
    {
    }
    void Init()
    {
        // 为了兼容和方便后续编写select代码，这里需要将accept返回值改为int，直接返回套接字文件描述符就好了
        _socket->BulidTcpServerMethod(_port);
    }
    void Start()
    {
        _isrunning = true;
        while (true)
        {
            // 以前是直接使用accept，accept去阻塞式等待
            // 现在从已经学过的IO的思想去看待accept
            //  IO = 等 + 拷贝
            // 之间的IO都是从系统中拷贝数据，这里的accept从listensockfd拿到连接,本质也是一种读取，所以下面使用读取的位图集
            // 学了select以后，直接使用select去检测listensockfd里面有没有新的连接
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(_socket->GetFd(), &rfds);
            // int select (int __nfds, fd_set *__restrict __readfds,fd_set *__restrict __writefds,fd_set *__restrict __exceptfds,struct timeval *__restrict __timeout);
            struct timeval tm = {5, 0};
            int n = ::select(_socket->GetFd() + 1, &rfds, nullptr, nullptr, &tm);
            switch (n)
            {
            case 0: // 设置时间检测，但是都没有就绪
            {
                std::cout << "Event not ready" << std::endl;
                break;
            }
            case -1: // 发生错误
            {
                perror("select");
                break;
            }
            default:
                EnTask();
                break;
            }
            // Inet_addr client;
            // int newfd = _socket->AcceptOrDie(&client);
            // if (newfd < 0)
            // {
            //     //....等等
            // }
        }
        _isrunning = false;
    }
    void EnTask()
    {
        Inet_addr client;
        int sockfd = _socket->AcceptOrDie(&client);
        std::cout<<"new client is comming,sockfd: "<<sockfd<<",addr: "<<client.GetIp()<<std::endl;
    }
    ~SelectServer()
    {}
};
