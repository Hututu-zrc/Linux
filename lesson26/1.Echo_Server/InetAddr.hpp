#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Common.hpp"

// 该类有两个功能
// 1、将收到的struct sockaddr里面的数据放到下面的这个类里面，可以打印
// 2、根据port初始化sockaddr，用于服务器bind
class Inet_addr
{
private:
    void PortNetToHost()
    {
        _port = ::ntohs(_addr.sin_port);
    }
    void IpNetToHost()
    {
        // const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
        char ipbuff[64];
        _ip = ::inet_ntop(AF_INET, &_addr.sin_addr, ipbuff, sizeof(ipbuff));
    }

public:
    Inet_addr()
    {
    }
    Inet_addr(const struct sockaddr_in &addr) : _addr(addr)
    {
        PortNetToHost();
        IpNetToHost();
    }
    //下面的这个构造函数只适用于服务器初始化的时候
    Inet_addr(uint16_t port) : _port(port), _ip("")
    {
        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(_port);
        _addr.sin_addr.s_addr = INADDR_ANY;
    }
    bool operator==(const Inet_addr &addr) const
    {
        return _port == addr._port && _ip == addr._ip;
    }
    struct sockaddr *GetAddr()
    {
        return CONV(&_addr);
    }
    int GetLen()const 
    {
        return sizeof(_addr);
    }
    uint16_t GetPort()const
    {
        return _port;
    }
    std::string GetIp() const
    {
        return _ip;
    }
    ~Inet_addr()
    {
    }

private:
    struct sockaddr_in _addr;
    std::string _ip;
    uint16_t _port;
};