#pragma once

// 系统调用接口库
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 库函数
#include <string.h>
#include <error.h>
#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include <functional>

// 自己封装的库
#include "Mutex.hpp"
#include "Log.hpp"
#include "Common.hpp"
#include "InetAddr.hpp"
#include "User.hpp"
#include "Thread.hpp"
#include "ThreadPool.hpp"

using namespace LogModule;
using namespace ThreadPoolModule;
using namespace ThreadModule;
const static int default_gsocketfd = -1;
// const static std::string default_ip = "127.0.0.1";
const static uint16_t default_port = 8888;
// static量，只能被声明它的那个文件中的所有函数访问
// 这意味着即使在不同的文件中也有同名的 static const int 变量，它们也是独立的，互不影响

const static std::string ExitMessage = "我走了，你们聊";
using adduser_t = std::function<void(const Inet_addr &user)>;
using route_t = std::function<void(int sockfd, const std::string &msg)>;
using remove_t = std::function<void(const Inet_addr &user)>;
using task_t = std::function<void()>;

class UdpServer
{

public:
    UdpServer(uint16_t port = default_port)
        : _socket_fd(default_gsocketfd),
          _addr(port),
          _isrunning(false)

    {
    }

    void RegisterService(adduser_t adduser, route_t route, remove_t remove)
    {
        _adduser = adduser;
        _route = route;
        _reomve = remove;
    }
    void InitServer()
    {
        // 1.创建套接字 socket文件描述符
        // 第一个参数是域或者协议家族，这里代表的是网络传输IPv4 Internet protocols
        // 第二个参数是我们传输方式，这里是创建用户数据报
        // 第三个参数默认设置为0
        // int socket(int domain, int type, int protocol);
        _socket_fd = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (_socket_fd < 0)
        {
            LOG(LogLevel::FATAL) << strerror(errno);
            Die(Err::SOCKET_ERR);
        }
        // LOG(LogLevel::DEBUG)<<"_socketfd: "<<_socket_fd;

        // 2.配置套接字里面的属性 绑定端口号 告诉系统基本的信息
        // int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
        // 第一个参数是打开的socket文件
        // 第二个参数是套接字里面的属性结构体，主要是地址类型+端口号+ip地址
        // 第三个参数是第二个参数里面的长度
        // struct sockaddr的初始化操作

        // struct sockaddr_in local; // 地址类型+端口号+ip地址

        // // 这个参数首先要清零，然后对里面的数据进行赋值，因为结构体不能集体赋值

        // // 这个函数直接将指针里面指向的内容全部清零，也可以使用memset
        // bzero(&local, sizeof(local));
        // local.sin_family = AF_INET;
        // // 这里还需要将主机字节序，转变为网络字节序
        // // uint16_t htons(uint16_t __hostshort) noexcept(true)
        // local.sin_port = ::htons(_port);

        // // in_addr_t inet_addr(const char *__cp) noexcept(true)
        // // in_addr_t 其实就是32位的整数，因为ipv4有32位
        // local.sin_addr.s_addr = ::inet_addr(_ip.c_str());

        int n = ::bind(_socket_fd, CONV(_addr.GetAddr()), _addr.GetLen());
        if (n != 0)
        {
            LOG(LogLevel::FATAL) << strerror(errno);
            Die(Err::BIND_ERR);
        }
        // LOG(LogLevel::DEBUG)<<"_socketfd: "<<_addr.GetIp();

        //  3.激活套接字 //告诉网络socket的信息
        // 属于start函数里面的了
    }
    void Start()
    {
        // LOG(LogLevel::DEBUG)<<"port :"<<_addr.GetPort();
        if (_isrunning)
            return;
        _isrunning = true;
        // LOG(LogLevel::DEBUG)<<"_socketfd: "<<_socket_fd;

        // 服务器端启动以后就不需要停止的
        while (true)
        {
            // ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
            // 这个函数主要用来接收数据,在这里的话就是用来接收客户端发送来的数据
            // sockfd属于全双工，既可以同时发送也可以接收
            //  size_t len 一般就是sizeof(buff)-1，留一个字符给'\0'
            // struct sockaddr *src_addr 这个参数属于输入型参数，需要自己定义变量接收
            char buff[1024];

            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n = recvfrom(_socket_fd, buff, sizeof(buff) - 1, 0, CONV(&peer), &len);

            if (n < 0)
            {
                LOG(LogLevel::FATAL) << strerror(errno);
                Die(Err::RECVFROM_ERR);
            }

            // ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
            // const struct sockaddr *dest_addr 就是recvfrom拿到的结构体
            // std::cout<<buff<<std::endl;

            // struct sockaddr_in *Rev = (struct sockaddr_in *)(&peer);
            // std::string revip(::inet_ntoa(Rev->sin_addr));
            // uint16_t port = ntohs(Rev->sin_port);
            Inet_addr Rev(peer);

            // 开始判断，接收到的是不是新用户，如果是新用户就添加，如果不是就转发
            _adduser(Rev);

            // LOG(LogLevel::DEBUG) << " " << Rev.GetIp() << " " << Rev.GetPort();
            // recvfrom 函数返回实际接收到的字节数 n，但它并不会在接收到的数据末尾自动添加字符串结束符 '\0'。
            // 也就是说，buff 数组中存储的只是原始的二进制数据，没有以 '\0' 结尾。
            buff[n] = 0;

            
            std::string echo_message = "Echo# ";

            if (strcmp(buff,"QUIT")==0)
            {

                _reomve(Rev);
                echo_message += "我走了，你们聊";
                LOG(LogLevel::INFO) << echo_message;
                std::cout << echo_message << std::endl;
            }
            else
            {
                echo_message += buff;
            }

            // std::cout << echo_message << std::endl;
            // 这里是简单的分发任务
            LOG(LogLevel::DEBUG) << " " << Rev.GetIp() << " " << Rev.GetPort()<< ": "<<echo_message;

            task_t f = std::bind(_route, _socket_fd, echo_message);
            Threadpool<task_t>::CreateSingleThreadPool()->Equeue(f);
            _route(_socket_fd, echo_message);
            // ssize_t m = ::sendto(_socket_fd, echo_message.c_str(), echo_message.size(), 0, CONV(&peer), sizeof(peer));
            // if (m < 0)
            // {
            //     LOG(LogLevel::FATAL) << strerror(errno);
            //     Die(Err::SENDTO_ERR);
            // }
        }
        _isrunning = false;
    }
    int GetSocketFd()
    {
        return _socket_fd;
    }
    ~UdpServer()
    {
        if (_socket_fd > default_gsocketfd)
        {
            close(_socket_fd);
        }
    }

private:
    int _socket_fd; // 临界资源
    // 服务器端不需要限定ip地址的，只需要限定端口就行了
    // 在很多情况下，服务器并不关心连接是从哪个网络接口进来的，只需要监听所有可用的网络接口。
    // 一个进程可以绑定多个端口，一个端口只能绑定一个进程
    Inet_addr _addr; // 这里是服务器的地址
    bool _isrunning; // 服务器是否启动

    adduser_t _adduser; // 添加用户
    route_t _route;     //
    remove_t _reomve;
};