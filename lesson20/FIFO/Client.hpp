#pragma once
#include <iostream>
#include "Common.hpp"

// 命名管道的创建，主要是先创建管道，我们的两个进程，一个Server创建管道并且负责接收，一个Client发送
class Client
{
public:
    Client()
    {
    }
    bool OpenPipeForWrite()
    {
        _fd = OpenPipe(g_open_mode_write);
        if (_fd < 0)
        {
            return false;
        }
        return true;
    }

    void SendPipe(const std::string &in)
    {
        ::write(_fd, in.c_str(), in.size());
    }

    void ClosePipeForWrite()
    {
        ClosePipe(_fd);
    }

    ~Client()
    {
    }

private:
    int _fd = -1;
};