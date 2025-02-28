#pragma once

#include <sys/types.h> /* See NOTES */
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>
#include <functional>
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
    using func_t = std::function<void *(void *)>;
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
        LOG(LogLevel::INFO) << "listen is sucess,_listensockfd: " << _listensockfd;
    }
    void HandlerRequest(int sockfd)
    {
        LOG(LogLevel::INFO) << "HandlerRequest is sucess,sockfd: " << sockfd;

        char buff[1024];
        while (true)
        {

            memset(buff, 0, sizeof(buff));
            int n = ::read(sockfd, buff, sizeof(buff) - 1);
            LOG(LogLevel::INFO) << "buff: " << buff;
            // read读不到数据的时候，返回-1，所以这里死循环
            if (n > 0)
            {
                buff[n] = 0;
                std::string echomessage("Echo# ");
                echomessage += buff;
                ::write(sockfd, echomessage.c_str(), echomessage.size());
            }
            else if (n == 0)
            {
                LOG(LogLevel::INFO) << "Client exit";
                break;
            }
            else
            {
                break;
            }
        }
        ::close(sockfd);
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

            // 我们获取client的信息：数据（sockfd）+client socket信息（accept || recvfrom）
            int sockfd = accept(_listensockfd, CONV(&peer), &peerlen);
            if (sockfd < 0)
            {
                LOG(LogLevel::FATAL) << strerror(errno);

                continue;
            }
            LOG(LogLevel::INFO) << "accept is sucess,sockfd: " << sockfd;

            // //version_0 版本  无法同时处理多个客户端
            // HandlerRequest(sockfd);

            // version_1 版本 多进程版本
            //父子进程使用不同的文件描述符表，子进程会拷贝文件描述符表
            //解决问题1：关闭不需要的文件描述符表
            //解决问题2：HandlerRequest里面是死循环收发信息，如果不退出，父进程阻塞等待，还是只能处理一个客户端
            // 子进程去执行任务
            int id = fork();
            if (id < 0)
            {
                LOG(LogLevel::FATAL) << strerror(errno);
                break;
            }
            else if (id == 0)
            {
                LOG(LogLevel::INFO) << "SubProcess success";
                HandlerRequest(sockfd);
                exit(0);
            }

            int rid = ::waitpid(id,nullptr,0);
            if(rid<0)
            {
                LOG(LogLevel::FATAL)<<"waitpid error";
                break;
            }
        }
        _isrunning = false;
    }
    ~TcpServer() {}

private:
    // 进程的文件描述符是有限的，所以不需要的时候记得关闭
    // Linux支持对文件描述符进行扩展的，ulimit -a 最多扩展到64kb个，核心就是结构体里面存在动态数组，可以扩展

    int _listensockfd;
    int _port; // 用户输入的端口号

    Inet_addr _addr; // 服务器内部的属性
    bool _isrunning; // 判断服务器是否启动
    func_t _fun;
};