#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <sys/epoll.h>

#include "Socket.hpp"
#include "Epoller.hpp"
#include "Connection.hpp"

using namespace EpollerModule;
using namespace SocketModule;
#define MAX 4096
const int gdefaultfd = -1;
using connection_t = std::shared_ptr<Connection>;
class EpollServer // epollserver只关心connection
{
    static const int RECV_NUM = 64;

private:
    std::unique_ptr<Epoller> _epoll;
    bool _isrunning; // epoll模型的文件描述符
    // 将sockfd和对应的connection指针连接映射关系保存起来
    // connection指针指向的派生类对象Listener，里面调用的函数都是Listener函数
    std::unordered_map<int, connection_t> _connections; //
    struct epoll_event _rcev[RECV_NUM];                 // epoller里面的wait函数所需要的事件数组

public:
    EpollServer()
        : _epoll(std::make_unique<Epoller>()),
          _isrunning(false)
    {
    }

    void Init()
    {
        _epoll->Create();
    }

    // 对连接做的增删改查，这里是增加
    void InsertConnection(connection_t conn)
    {
        if (IsConnectionExists(conn->GetSockfd()) == true)
            return;
        // 将新的connection连接插入的unordered_map里面
        _connections.insert({conn->GetSockfd(), conn});
        // 将新的连接写入到内核
        _epoll->Ctl(EPOLL_CTL_ADD, conn->GetSockfd(), conn->GetEvents());
        // 这里插入的时候就直接设置EpollServer回调指针
        conn->SetEpollOwner(this);
    }
    bool IsConnectionExists(int sockfd) // 判断插入的connection连接是否存在
    {
        return _connections.find(sockfd) != _connections.end();
    }
    void DisPatch(int n)
    {

        for (int i = 0; i < n; i++)
        {
            int sockfd = _rcev[i].data.fd;
            uint32_t revents = _rcev[i].events;
            if ((revents & EPOLLERR) || (revents & EPOLLHUP))
            {
                // 将错误事件转换为读写事件//？？？？
                revents = EPOLLIN | EPOLLOUT;
            }
            // 读事件，并且map里面存在这连接
            if ((revents & EPOLLIN) && IsConnectionExists(sockfd))
            {
                // _connections[sockfd]->CallRecv();
                // _connections[sockfd]->Who();
                _connections[sockfd]->Recv();
            }
            if ((revents & EPOLLOUT) && IsConnectionExists(sockfd))
            {
                // _connections[sockfd]->CallRecv();
                _connections[sockfd]->Send();
            }
        }
    }
    void LoopOnce()
    {
        int timeout = -1;

        int n = _epoll->Wait(_rcev, RECV_NUM, timeout);
        DisPatch(n);
    }
    void Loop()
    {
        _isrunning = true;
        while (_isrunning)
        {
            LoopOnce();
        }
        _isrunning = false;
    }

    ~EpollServer() {}
};
// void Init()
// {
//     // 为了兼容和方便后续编写select代码，这里需要将accept返回值改为int，直接返回套接字文件描述符就好了
//     _socket->BulidTcpServerMethod(_port);
//     _epfd = ::epoll_create(256);
//     if (_epfd < 0)
//     {
//         LOG(LogLevel::ERROR) << "epoll_create error!";
//         Die(EPOLL_CREATE_ERR);
//     }
//     LOG(LogLevel::DEBUG) << "epoll_create success,fd"<<_epfd;
//     struct epoll_event in;
//     in.events = EPOLLIN;
//     in.data.fd = _socket->GetFd();
//     int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, _socket->GetFd(), &in);
//     if (n < 0)
//     {
//         LOG(LogLevel::ERROR) << "epoll_ctl error!";
//         Die(EPOLL_CTL_ERR);
//     }
//     LOG(LogLevel::DEBUG) << "epoll_ctl success!";
// }
// void Loop()
// {

//     _isrunning = true;
//     int timeout = -1;
//     while (_isrunning)
//     {

//         int n = ::epoll_wait(_epfd, _rcev, RECV_NUM, timeout);
//         switch (n)
//         {
//         case 0: // 设置时间检测，但是都没有就绪
//         {
//             std::cout << "Event not ready" << std::endl;
//             sleep(1);
//             break;
//         }
//         case -1: // 发生错误
//         {
//             perror("select");
//             break;
//         }
//         default:
//             // Dispatcher(n); // 进行派发任务
//             break;
//         }
//     }
//     _isrunning = false;
// }

// void Accepter() // 回调函数呢？？
// {
//     Inet_addr client;
//     int newfd = _socket->AcceptOrDie(&client); // 不会阻塞
//     std::cout << "new client is comming,sockfd: " << newfd << ",addr: " << client.GetIp() << std::endl;
//     if (newfd < 0)
//         return;
//     // 将新的fd添加到epoll模型当中
//     struct epoll_event in;
//     in.data.fd = newfd;
//     in.events = EPOLLIN;
//     int n = ::epoll_ctl(_epfd, EPOLL_CTL_ADD, newfd, &in);
//     if (n < 0)
//     {
//         LOG(LogLevel::ERROR) << "epoll_ctl error!";
//         Die(EPOLL_CTL_ERR);
//     }
//     LOG(LogLevel::DEBUG) << "epoll_ctl success!";
// }
// void Recver(int fd) // 回调函数呢？？
// {
//     char buff[1024];
//     int n = recv(fd, buff, sizeof(buff) - 1, 0); // 这里其实是存在问题，可能读取不完整，但是这里先不管
//     // 只有出错误或者客户端关闭连接，才需要关闭文件描述符
//     if (n < 0)
//     {
//         LOG(LogLevel::ERROR) << "recv error";
//         int m = ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
//         if (m < 0)
//         {

//             LOG(LogLevel::ERROR) << "epoll_ctl error!";
//             return;
//         }
//         LOG(LogLevel::DEBUG) << "epoll_ctl success!";
//         ::close(fd);
//     }
//     else if (n == 0) // 用户断开连接
//     {
//         //_sockfds删除
//         LOG(LogLevel::INFO) << "recv exit";
//         // epoll模型当中删除fd事件，fd必须是合法的。
//         // 所以先删除epoll模型里面的fd，然后关闭文件描述符
//         int m = ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
//         if (m < 0)
//         {
//             LOG(LogLevel::ERROR) << "epoll_ctl error!";
//             return;
//         }
//         LOG(LogLevel::DEBUG) << "epoll_ctl success!";
//         ::close(fd);
//     }
//     else // 读取成功
//     {
//         buff[n] = 0;
//         std::cout << "client# " << buff << std::endl;
//         // 数据处理后返回，这里暂时就是直接返回回显
//         std::string message = "echo# ";
//         message += buff;
//         send(fd, message.c_str(), message.size(), 0);
//         // 如果需要持续的读，那么就不能关闭该文件描述符
//         // 必须客户端结束连接，然后走else if(n==0)的情况，此时就会关闭文件描述符
//     }
// }
// void Dispatcher(int rnum)
// {

//     LOG(LogLevel::DEBUG) << "Dispatcher begin";
//     for (int i = 0; i < rnum; i++)
//     {
//         // 遇到的是listen sock fd
//         int events=_rcev[i].events;
//         int fd=_rcev[i].data.fd;
//         if (fd == _socket->GetFd())
//         {

//             if ( events& EPOLLIN) // 并且在读集合里面是合法的
//             {
//                 Accepter(); // 连接的获取
//             }
//         }
//         else
//         {
//             // 其他合法就绪的事件
//             if (events & EPOLLIN)
//             {
//                 Recver(fd); // IO的处理
//             }
//         }
//     }
//     LOG(LogLevel::DEBUG) << "Dispatcher end";
// }

// ~EpollServer()
// {
//     _socket->Close();
//     if (_epfd >= 0)
//         ::close(_epfd);
// }
