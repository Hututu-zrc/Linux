#pragma once
#include <iostream>
#include <memory>
#include "Socket.hpp"
using namespace SocketModule;

// 专门负责获取链接的模块
class Listener
{
public:
    Listener(uint16_t port = 8888)
        : _port(port),
          _listensock(std::make_shared<Socket>())
    {
    }
    void Init()
    {
        _listensock->BulidTcpServerMethod(_port);
    }
    int GetSockFd()
    {
        return _listensock->GetFd();
    }
    void Accept()
    {
        LOG(LogLevel::DEBUG) << "Accept";
        // _listensock->AcceptOrDie();
    }
    ~Listener() {}

private:
    uint16_t _port;
    std::shared_ptr<Socket> _listensock;
};