#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Common.hpp"

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
    Inet_addr(uint16_t port):_port(port),_ip("")
    {
        _addr.sin_family=AF_INET;
        _addr.sin_port= htons(_port);
        _addr.sin_addr.s_addr=INADDR_ANY;
    }

    struct sockaddr *GetAddr()
    {
        return CONV(&_addr);
    }
    int GetLen()
    {
        return sizeof(_addr);
    }
    uint16_t GetPort()
    {
        return _port;
    }
    std::string GetIp()
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