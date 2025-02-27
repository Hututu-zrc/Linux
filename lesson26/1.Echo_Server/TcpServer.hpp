#pragma once

#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>
#include <iostream>

#include "Log.hpp"
#include "Common.hpp"
#include "InetAddr.hpp"

#define BACKLOG 5
using namespace LogModule;
static const int gport = 8888;

class TcpServer
{
public:
    TcpServer(int port = gport)
        : _listensockfd(-1),
          _port(port),

          _addr(_port),
          _isrunning(false)
    {
    }
    void InitServer()
    {
        _listensockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << strerror(errno);
            Die(SOCKET_ERR);
        }

        int n = ::bind(_listensockfd, CONV(&_addr), sizeof(_addr));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << strerror(errno);
            Die(BIND_ERR);
        }

        // 由于TCP要先建立连接，如果多个client发送信息的话，需要排队等待
        int m = ::listen(_listensockfd, BACKLOG);
        if (m < 0)
        {
            LOG(LogLevel::FATAL) << strerror(errno);
            Die(LISTEN_ERR);
        }
        LOG(LogLevel::INFO) << "listen is sucess " << _listensockfd;
    }
    void HandlerRequest(int sockfd)
    {
        LOG(LogLevel::INFO) << "HandlerRequest is sucess " << _listensockfd;

        char buff[1024];
        while (true)
        {

            memset(buff, 0, sizeof(buff));
            int n = ::read(sockfd, buff, sizeof(buff) - 1);
            //read读不到数据的时候，返回-1，所以这里死循环
            if (n > 0)
            {
                buff[n] = 0;
                std::string echomessage("Echo# ");
                echomessage += buff;
                ::write(sockfd, echomessage.c_str(), echomessage.size());
            }
        }
    }
    void Start()
    {
        if (_isrunning)
            return;
        _isrunning = true;
        while (true)
        {
            // int accept(int sockfd, struct sockaddr *addr,socklen_t *addrlen, int flags);
            struct sockaddr_in peer;

            socklen_t peerlen = sizeof(peer); // 初始化未peer的大小
            int sockfd = accept(_listensockfd, CONV(&peer), &peerlen);
            if (sockfd < 0)
            {
                LOG(LogLevel::FATAL) << strerror(errno);

                continue;
            }
            LOG(LogLevel::INFO) << "accept is sucess " << _listensockfd;

            HandlerRequest(sockfd);
        }
        _isrunning = false;
    }
    ~TcpServer() {}

private:
    int _listensockfd;
    int _port; // 用户输入的端口号

    Inet_addr _addr; // 服务器内部的属性
    bool _isrunning; // 判断服务器是否启动
};