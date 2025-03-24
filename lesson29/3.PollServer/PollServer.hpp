#pragma once
#include <iostream>
#include <string>
#include <poll.h>
#include "Socket.hpp"

using namespace SocketModule;
#define MAX 4096
const int gdefaultfd = -1;

class PollServer
{
private:
    uint16_t _port;
    std::unique_ptr<Socket> _socket;
    bool _isrunning;
    struct pollfd _sockfds[MAX]; // 辅助数组来保存文件描述符

public:
PollServer(std::string port)
        : _port(stoi(port)),
          _socket(std::make_unique<TcpSocket>()),
          _isrunning(false)
    {
    }
    void Init()
    {
        // 为了兼容和方便后续编写select代码，这里需要将accept返回值改为int，直接返回套接字文件描述符就好了
        _socket->BulidTcpServerMethod(_port);

        // 要将sockfds数组首先全部默认初始化为-1
        for (int i = 0; i < MAX; i++)
        {
            _sockfds[i].fd = gdefaultfd;
            _sockfds[i].events = 0;
            _sockfds[i].revents = 0;
        }
        _sockfds[0].fd = _socket->GetFd(); // 第一个文件描述符默认存储为监听套接字
        _sockfds[0].events = POLLIN;
    }
    void Loop()
    {

        _isrunning = true;
        while (true)
        {

            // int timelen=0;
            // poll的改进地方就是设置集合和返回集合分成两个events和revents了
            // 每次不需要对集合进行重新赋值
            // int poll(struct pollfd fds[], nfds_t nfds, int timeout);
            // 底层还是遍历poll的数组，看有没有准备好的文件
            int n = ::poll(_sockfds, MAX, 0);
            switch (n)
            {
            case 0: // 设置时间检测，但是都没有就绪
            {
                std::cout << "Event not ready" << std::endl;
                sleep(1);
                break;
            }
            case -1: // 发生错误
            {
                perror("select");
                break;
            }
            default:
                Dispatcher(); // 进行派发任务
                break;
            }
        }
        _isrunning = false;
    }

    void Accepter() // 回调函数呢？？
    {
        Inet_addr client;
        int newfd = _socket->AcceptOrDie(&client);
        std::cout << "new client is comming,sockfd: " << newfd << ",addr: " << client.GetIp() << std::endl;
        if (newfd < 0)
            return;
        // 将新的newfd添加到辅助数组当中，并且写入读集
        // 不管是poll还是select这里加入新的newfd都需要遍历
        int pos = gdefaultfd;
        for (int j = 0; j < MAX; j++)
        {
            if (_sockfds[j].fd != gdefaultfd)
            {
                pos = j;
                break;
            }
        }
        if (pos == -1)
        {
            // 证明当前的服务器select管理已达到上限
            LOG(LogLevel::ERROR) << "服务器连接已达到上限";
            //这里可以扩容，不过要修改成员变量，比如struct pollfd _sockfds改为strcut pollfd * _sockfd
            //可以使用malloc扩容
            // 关闭文件描述符
            ::close(newfd);
        }
        else
        {
            LOG(LogLevel::INFO) << "newfd加入_sockfds";
            _sockfds[pos].fd = newfd;
            _sockfds[pos].events = POLLIN;
        }
    }
    void Recver(int who) // 回调函数呢？？
    {
        char buff[1024];
        int n = recv(_sockfds[who].fd, buff, sizeof(buff) - 1, 0); // 这里其实是存在问题，可能读取不完整，但是这里先不管
        // 只有出错误或者客户端关闭连接，才需要关闭文件描述符
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << "recv error";
            ::close(_sockfds[who].fd);
            _sockfds[who].fd = gdefaultfd;
            Die(RECV_ERR);
        }
        else if (n == 0) // 用户断开连接
        {
            //_sockfds删除
            LOG(LogLevel::INFO) << "recv exit";

            ::close(_sockfds[who].fd);
            _sockfds[who].fd = gdefaultfd;
        }
        else // 读取成功
        {
            buff[n] = 0;
            std::cout << "client# " << buff << std::endl;
            // 数据处理后返回，这里暂时就是直接返回回显
            std::string message = "echo# ";
            message += buff;
            send(_sockfds[who].fd, message.c_str(), message.size(), 0);
            // 如果需要持续的读，那么就不能关闭该文件描述符
            // 必须客户端结束连接，然后走else if(n==0)的情况，此时就会关闭文件描述符
        }
    }
    void Dispatcher()
    {

        // 这里相当于每次有新的连接，等到下个循环才会开始使用
        // select不停的接收新的连接，然后存到辅助数组里里面
        // 辅助数组里面的文件描述符越多，代表select控制的连接越多，有点相当于多线程的效果
        // 每次进入的Recver函数里面，就会遍历所有的文件描述符，然后接收消息，知道客户端关闭连接，此时才删除该文件描述符
        LOG(LogLevel::DEBUG) << "Dispatcher";
        for (int i = 0; i < MAX; i++)
        {
            if (_sockfds[i].fd == gdefaultfd)
                continue;
            if (_sockfds[i].fd == _socket->GetFd()) // 遇到的是listensockfd
            {
                //这里判断OS内核里面写入的revents
                if (_sockfds[i].revents & POLLIN) // 并且在读集合里面是合法的
                {
                    Accepter(); // 连接的获取
                }
            }
            else
            {
                // 遇到的是_sockfds数组里面的存在的，然后判断在不在读集合当中
                if (_sockfds[i].revents & POLLIN)
                {
                    Recver(i); // IO的处理
                }
            }
        }
    }
    ~PollServer()
    {
    }
};
