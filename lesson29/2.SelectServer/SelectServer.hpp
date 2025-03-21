#pragma once
#include <iostream>
#include <string>
#include <sys/select.h>
#include "Socket.hpp"

using namespace SocketModule;
#define NUM 1024
const int gdefaultfd = -1;


class SelectServer
{
private:
    uint16_t _port;
    std::unique_ptr<Socket> _socket;
    bool _isrunning;
    int _sockfds[NUM]; // 辅助数组来保存文件描述符

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

        // 要将sockfds数组首先全部默认初始化为-1
        for (int i = 0; i < NUM; i++)
        {
            _sockfds[i] = gdefaultfd;
        }
        _sockfds[0] = _socket->GetFd(); // 第一个文件描述符默认存储为监听套接字
    }
    void Loop()
    {
        fd_set rfds;
        _isrunning = true;
        while (true)
        {
            // 以前是直接使用accept，accept去阻塞式等待
            // 现在从已经学过的IO的思想去看待accept
            //  IO = 等 + 拷贝
            // 之间的IO都是从系统中拷贝数据，这里的accept从listensockfd拿到连接,本质也是一种读取，所以下面使用读取的位图集
            // 学了select以后，直接使用select去检测listensockfd里面有没有新的连接

            // 由于select由OS检查以后会返回被OS修改后的读集合，所以每次都要重新注册
            // 这里使用辅助数组来保存所有的文件描述符

            FD_ZERO(&rfds);
            int maxfd = gdefaultfd;
            for (int i = 0; i < NUM; i++)
            {

                if (_sockfds[i] == gdefaultfd)
                    continue;
                FD_SET(_sockfds[i], &rfds);
                // 辅助数组里面存储的情况可能是{4,8,7,6,5};其中4是listensockfd
                // 辅助数组里面不是一定按照从小到大排序的
                if (maxfd < _sockfds[i])
                    maxfd = _sockfds[i];
            }
            // int select (int __nfds, fd_set *__restrict __readfds,fd_set *__restrict __writefds,fd_set *__restrict __exceptfds,struct timeval *__restrict __timeout);
            struct timeval tm = {5, 0};
            // select准备好就通知上层
            int n = ::select(maxfd + 1, &rfds, nullptr, nullptr, &tm);
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
                Dispatcher(rfds); // 进行派发任务
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

    void Accepter() // 回调函数呢？？
    {
        Inet_addr client;
        int newfd = _socket->AcceptOrDie(&client);
        std::cout << "new client is comming,sockfd: " << newfd << ",addr: " << client.GetIp() << std::endl;
        if (newfd < 0)
            return;
        // 将新的newfd添加到辅助数组当中，并且写入读集
        int pos = gdefaultfd;
        for (int j = 0; j < NUM; j++)
        {
            if (_sockfds[j] != gdefaultfd)
            {
                pos = j;
                break;
            }
        }
        if (pos == -1)
        {
            // 证明当前的服务器select管理已达到上限
            LOG(LogLevel::ERROR) << "服务器连接已达到上限";
            // 关闭文件描述符
            ::close(newfd);
        }
        else
        {
            LOG(LogLevel::INFO) << "newfd加入_sockfds";
            _sockfds[pos] = newfd;
            // 这里不需要直接设置进入读集合里面，直接等待整个for循环走完，下次再通过_sockfds写入读集合里面
            // 实验可以得出结论，entask进入了两次
            //  FD_SET(_sockfds[pos], &rfds);
        }
    }
    void Recver(int who) // 回调函数呢？？
    {
        char buff[1024];
        int n = recv(_sockfds[who], buff, sizeof(buff) - 1, 0); // 这里其实是存在问题，可能读取不完整，但是这里先不管
        //只有出错误或者客户端关闭连接，才需要关闭文件描述符
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << "recv error";
            ::close(_sockfds[who]);
            _sockfds[who] = gdefaultfd;
            Die(RECV_ERR);
        }
        else if (n == 0) // 用户断开连接
        {
            //_sockfds删除
            LOG(LogLevel::INFO) << "recv exit";

            ::close(_sockfds[who]);
            _sockfds[who] = gdefaultfd;
        }
        else // 读取成功
        {
            buff[n] = 0;
            std::cout << "client# " << buff << std::endl;
            // 数据处理后返回，这里暂时就是直接返回回显
            std::string message = "echo# ";
            message += buff;
            send(_sockfds[who], message.c_str(), message.size(), 0);
            // 如果需要持续的读，那么就不能关闭该文件描述符
            //必须客户端结束连接，然后走else if(n==0)的情况，此时就会关闭文件描述符
        }
    }
    void Dispatcher(fd_set &rfds)
    {

        // 这里相当于每次有新的连接，等到下个循环才会开始使用
        // select不停的接收新的连接，然后存到辅助数组里里面
        // 辅助数组里面的文件描述符越多，代表select控制的连接越多，有点相当于多线程的效果
        // 每次进入的Recver函数里面，就会遍历所有的文件描述符，然后接收消息，知道客户端关闭连接，此时才删除该文件描述符
        LOG(LogLevel::DEBUG) << "EnTask";
        for (int i = 0; i < NUM; i++)
        {
            if (_sockfds[i] == gdefaultfd)
                continue;
            if (_sockfds[i] == _socket->GetFd()) // 遇到的是listensockfd
            {
                if (FD_ISSET(_socket->GetFd(), &rfds)) // 并且在读集合里面是合法的
                {
                    Accepter(); // 连接的获取
                }
            }
            else
            {
                // 遇到的是_sockfds数组里面的存在的，然后判断在不在读集合当中
                if (FD_ISSET(_sockfds[i], &rfds))
                {
                    Recver(i); // IO的处理
                }
            }
        }
    }
    ~SelectServer()
    {
    }
};
