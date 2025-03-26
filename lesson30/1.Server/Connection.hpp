#pragma once
#include <iostream>
#include <string>
#include <functional>
#include "InetAddr.hpp"

class EpollServer;
// 把所有的文件描述符做一下封装
class Connection
{
public:
    using func_t = std::function<void()>;
    // 那么初始化的时候，就应该指明sockfd和关心的事件
    Connection(int sockfd)
        : _sockfd(sockfd),
          _events(0)
    {
    }


    void Register(func_t recv, func_t send, func_t exception)
    {
        _recv = recv;
        _send = send;
        _exception = exception;
    }

    void SetEvents(uint32_t events)
    {
        _events = events;
    }
    uint32_t GetEvents()
    {
        return _events;
    }
    int GetSockfd()
    {
        return _sockfd;
    }
    void CallRecv()
    {
        _recv();
    }
    void CallSend()
    {
        _send();
    }
    void CallExcep()
    {
        _exception();
    }
    ~Connection() {}

private:
    int _sockfd;
    std::string _inbuffer;  // 读取时候的输入缓冲区
    std::string _outbuffer; // 发生时候的输出缓冲区
    Inet_addr _peer_addr;   // 对应的哪一个客户端

    // 回调方法
    func_t _recv;
    func_t _send;
    func_t _exception;

    // 添加一个指针，表明该connection属于哪个epoll服务器
    EpollServer *_owner;

    // 添加该文件描述符关心的事件
    uint32_t _events;
};