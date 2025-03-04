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
#include "ThreadPool.hpp"
#include "Protocol.hpp"

#define BACKLOG 5
using namespace LogModule;
using namespace ThreadPoolModule;
using handler_t = std::function<std::string(std::string &)>;
static const int gport = 8888;

class TcpServer
{
    struct Data
    {
        int *sockfd;
        TcpServer *self;
    };

public:
    // using func_t = std::function<void *(void *)>;
    using task_t = std::function<void()>;

    TcpServer(handler_t handler, int port = gport)
        : _listensockfd(-1),
          _port(port),
          _addr(_port),
          _isrunning(false),
          _handler(handler)

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
        std::string package; // 数据报文
        // 长任务
        while (true)
        {
            memset(buff, 0, sizeof(buff));
            ssize_t n = ::recv(sockfd, buff, sizeof(buff) - 1, 0);
            if (n > 0)
            {
                buff[n] = 0;
                package+=buff;
              
                
                
                std::string ret = _handler(package);
                // 判断数据报是否完整，不完整的话，直接退出，接着读取
                if(ret.empty()) continue;

                // ::write(sockfd, echomessage.c_str(), echomessage.size());//写入也是不完善的
                ::send(sockfd, ret.c_str(), ret.size(), 0); // 写入也是不完善的
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
        // LOG(LogLevel::DEBUG) << "sockfd close,sockfd: " << sockfd;
    }

    // 这里出现的问题就是，静态类内函数没有this指针
    // 解决方法就是：传递strcut结构体，然后结构体里面放个指针

    static void *Handler(void *args)
    {
        // 线程内部设置为分离状态，免得回收
        pthread_detach(pthread_self());

        Data *data = static_cast<Data *>(args);
        data->self->HandlerRequest(*data->sockfd);
        return nullptr;
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

            // // version_1 版本 多进程版本
            // // 父子进程使用不同的文件描述符表，子进程会拷贝文件描述符表
            // // 解决问题1：关闭不需要的文件描述符表  //已解决
            // // 解决问题2：HandlerRequest里面是死循环收发信息，
            // // 如果不退出，父进程阻塞等待，还是只能处理一个客户端
            // // 解决方法1：signal(SIGCHLD,SIG_IGN);
            // // 当父进程调用 signal(SIGCHLD, SIG_IGN); 后，内核会在子进程终止时自动回收其资源，
            // // 而不需要父进程调用 wait() 或 waitpid() 来处理，从而避免了僵尸进程的产生。
            // //解决方法2：采用子进程里面再次开辟进程，使用孙子进程去执行任务，让子进程退出，
            // //这样孤儿进程就可以直接被回收，不需要父进程等待了
            // // 子进程去执行任务
            // signal(SIGCHLD, SIG_IGN);
            // int id = fork();
            // if (id < 0)
            // {
            //     LOG(LogLevel::FATAL) << strerror(errno);
            //     break;
            // }
            // else if (id == 0)
            // {
            //     close(_listensockfd);
            //     LOG(LogLevel::INFO) << "SubProcess success";
            //     if(fork()==0)
            //     {
            //         HandlerRequest(sockfd);
            //     }
            //     exit(0);
            // }

            // close(sockfd);
            // int rid = ::waitpid(id, nullptr, 0);
            // if (rid < 0)
            // {
            //     LOG(LogLevel::FATAL) << "waitpid error";
            //     break;
            // }

            // // version_3 多线程版本
            // pthread_t tid;
            // // 因为线程之间是共享同一份文件描述符表的，所以这里使用的时候必须拷贝一份sockfd
            // // 不然多线程使用的时候，前面的线程还在使用，后面的新线程直接修改了sockfd
            // //这里和系统的分配策略有关系，关闭了的文件描述符不会立即启用
            // int *psockfd = new int(sockfd);
            // Data data;
            // data.self = this;
            // data.sockfd = psockfd;
            // // 这里的错误点就是Handler是类内函数，需要传递指针，否则就是改成静态类内成员函数
            // pthread_create(&tid, nullptr, Handler, &data);

            // version_4 线程池版本
            // 这里的sockfd是赋值拷贝
            // 线程池只适合短任务，不适合长任务

            Threadpool<task_t>::CreateSingleThreadPool()->Equeue([this, sockfd]()
                                                                 {
               // LOG(LogLevel::DEBUG)<<"lambda ";
                this->HandlerRequest(sockfd); });
        }
        _isrunning = false;
    }
    void Stop()
    {
        _isrunning = false;
        Threadpool<task_t>::CreateSingleThreadPool()->Stop();
        Threadpool<task_t>::CreateSingleThreadPool()->Wait();
    }
    ~TcpServer() {}

private:
    // 进程的文件描述符是有限的，所以不需要的时候记得关闭
    // Linux支持对文件描述符进行扩展的，ulimit -a 最多扩展到64kb个，核心就是结构体里面存在动态数组，可以扩展

    int _listensockfd;
    int _port; // 用户输入的端口号

    Inet_addr _addr; // 服务器内部的属性
    bool _isrunning; // 判断服务器是否启动

    // 处理上层任务的入口
    handler_t _handler;
};