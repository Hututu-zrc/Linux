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

// 这里是事件驱动的模式，将IO事件的响应和处理进行分离
/// 通过创建Epoll模型，得到epfd文件描述符，然后通过epoll模型去监听多个IO源上的事件
// 比如listensockfd，普通的连接fd等
// 这种架构模式叫做反应堆模式
class Reactor // epollserver只关心connection
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
    Reactor()
        : _epoll(std::make_unique<Epoller>()),
          _isrunning(false)
    {
    }

    void Init()
    {
        _epoll->Create();
    }
    void DeleteConnection(int sockfd)
    {
        if (IsConnectionExists(sockfd) == true)
        {

            // 删除内核里面的连接
            _epoll->Ctl(EPOLL_CTL_DEL, sockfd, _connections[sockfd]->GetEvents());

            // 关闭文件描述符
            ::close(sockfd);
            // 删除map里面的连接
            _connections.erase(sockfd);
        }
    }
    void ModifyConnection(int sockfd, bool read_flag, bool write_flag)
    {
        if (IsConnectionExists(sockfd) == true)
        {
            uint32_t event = (read_flag ? EPOLLIN : 0) | (write_flag ? EPOLLOUT : 0) | EPOLLET;
            _connections[sockfd]->SetEvents(event);
            _epoll->Ctl(EPOLL_CTL_MOD, sockfd, _connections[sockfd]->GetEvents());
        }
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
                // 将错误事件转换为读写事件//？？？
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

    ~Reactor() {}
};

