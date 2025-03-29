#pragma once
#include <iostream>
#include <memory>
#include <sys/epoll.h>
#include "Socket.hpp"
#include "Connection.hpp"
#include "IOService.hpp"
#include"Calculate.hpp"

using namespace SocketModule;
#include "Reactor.hpp"
// 专门负责获取链接的模块
// 将子类Listener继承父类的Connection
// 这样Listener后续使用就比较方便
// class EpollerServer;
class Listener : public Connection
{
public:
    // 初始化列表里面自动化调用connection的父类构造函数
    Listener(uint16_t port = 8888)
        : _port(port),
          _listensock(std::make_unique<TcpSocket>())
    {
        // BuildTcpServerMethod函数里面就已经设置为非阻塞了
        _listensock->BulidTcpServerMethod(_port);

        // 设置父类里面的函数成员变量
        // 这里就是插入的listensockfd
        // 将父类Connection的文件描述符
        SetSockfd(_listensock->GetFd());
        // 设置对应的文件描述符的事件类型
        SetEvents(EPOLLIN | EPOLLET);
    }

    int GetSockFd()
    {
        return _listensock->GetFd();
    }
    void Recv() override
    {
        LOG(LogLevel::DEBUG) << "Accept";
        // 在epollserver里面就已经固定死了，只有读事件就绪的时候，才会使用这个函数
        // IO处理，用来获取新的连接
        /// 这里必须循环读取，保证一次性读完
        while (true)
        {
            Inet_addr peer;
            int aerrno = 0;
            int sockfd = _listensock->AcceptOrDie(&peer, &aerrno);
            if (sockfd > 0)
            {
                // success
                LOG(LogLevel::DEBUG) << "Recv Success";
                // 这里另外写一个类，去实现普通的文件描述符的插入IOService

                // 1.设置为非阻塞的文件描述符
                //   SetNonBlock(sockfd);
                // 2.将sockfd构建为connection
                auto conn = std::make_shared<IOService>(sockfd);
                conn->RegisterOnMeassage(HandlerRequest);

                // 3.注册到Epoll里面，使用回调指针
                // listensockfd执行InsertConnection的时候，就已经设置了EpollServer的指针
                GetEpollOwner()->InsertConnection(conn);
                LOG(LogLevel::DEBUG) << "Add Connection Success!";
            }
            else
            {
                // 非阻塞的文件描述符获取失败，需要判断具体是什么情况导致的失败
                if (aerrno == EAGAIN || aerrno == EWOULDBLOCK) // 底层数据没有就绪
                {
                    LOG(LogLevel::DEBUG) << "accept all connection ... done";
                    break;
                }
                else if (aerrno == EINTR) // 读取过程中，被信号打断了
                {
                    LOG(LogLevel::DEBUG) << "accept interrupted by signal,continue";
                    continue;
                }
                else // 真正的读取失败，发生了错误
                {
                    LOG(LogLevel::DEBUG) << "accept error ... done,errno: " << aerrno;
                    break;
                }
            }
        }
        // _listensock->AcceptOrDie();
    }
    void Send() override
    {
    }
    void Excep() override
    {
    }
    void Who()
    {
        std::cout << "i am listener" << std::endl;
    }
    ~Listener() {}

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
};