#pragma once
#include <iostream>
#include <sys/epoll.h>
#include "Log.hpp"
using namespace LogModule;
namespace EpollerModule
{
    // 专门用来包装epoll的接口
    class Epoller
    {
    public:
        Epoller() : _epfd(-1) {}
        int Create()
        {
            _epfd = ::epoll_create(256);
            if (_epfd < 0)
            {
                LOG(LogLevel::ERROR) << "epoll_create error!";
                Die(EPOLL_CREATE_ERR);
            }
            LOG(LogLevel::DEBUG) << "epoll_create success,fd" << _epfd;
            return _epfd;
        }
        int Wait(struct epoll_event rcev[], int recv_num, int timeout)
        {
            int n = ::epoll_wait(_epfd, rcev, recv_num, timeout);
            if (n < 0)
            {
                LOG(LogLevel::ERROR) << "epoll_wait error!";
                Die(EPOLL_WAIT_ERR);
            }
            LOG(LogLevel::DEBUG) << "epoll_wait success,fd_num: " << n;
            return n;
        }
        int Ctl(int mode, int fd, uint32_t events)
        {
            // 上层只负责传递属性，不会创建struct epoll_event结构体
            // 将属性传递到底层的Epoller类，这时候才会创建结构体，来写入内核
            struct epoll_event event;
            event.data.fd = fd;
            event.events = events;
            int n = epoll_ctl(_epfd, mode, fd, &event);
            if (n < 0)
            {
                LOG(LogLevel::ERROR) << "epoll_ctl error!";
                Die(EPOLL_CTL_ERR);
            }
            LOG(LogLevel::DEBUG) << "epoll_ctl success!";
            return n;
        }
        ~Epoller() {}

    private:
        int _epfd;
    };
}