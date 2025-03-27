#pragma once
#include <iostream>
#include <string>
#include <functional>
#include "InetAddr.hpp"

class EpollServer;
// 把所有的文件描述符做一下封装
// 将这个函数作为基类函数，原本的回调函数变成接口函数
class Connection
{
public:
    using func_t = std::function<void()>;
    // 那么初始化的时候，就应该指明sockfd和关心的事件
    Connection()
        : _sockfd(-1),
          _events(0)
    {
    }
    void SetEpollOwner(EpollServer *epollserver_ptr)
    {
        _owner = epollserver_ptr;
    }
    EpollServer *GetEpollOwner()
    {
        return _owner;
    }
    void SetEvents(uint32_t events)
    {
        _events = events;
    }
    uint32_t GetEvents()
    {
        return _events;
    }
    void SetSockfd(int sockfd)
    {
        _sockfd = sockfd;
    }
    int GetSockfd()
    {
        return _sockfd;
    }
    virtual void Recv() = 0;
    virtual void Send() = 0;
    virtual void Excep() = 0;
    // virtual void Who()=0;

    ~Connection() {}

protected:
    int _sockfd;
    std::string _inbuffer;  // 读取时候的输入缓冲区
    std::string _outbuffer; // 发生时候的输出缓冲区
    Inet_addr _peer_addr;   // 对应的哪一个客户端

    // 添加一个指针，表明该connection属于哪个epoll服务器
    EpollServer *_owner;

    // 添加该文件描述符关心的事件
    uint32_t _events;
};