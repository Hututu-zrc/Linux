#pragma once
#include <iostream>
#include <memory>
#include <sys/epoll.h>
#include "Socket.hpp"
#include "Connection.hpp"
using namespace SocketModule;

using func_t = std::function<std::string(std::string &)>;
class IOService : public Connection
{

private:
    int _sockfd;
    func_t _onmessage;

public:
    static const int Size = 4096;
    IOService(int sockfd = -1)
        : _sockfd(sockfd)
    {
        // 1.设置为非阻塞的文件描述符
        SetNonBlock(sockfd);
        SetSockfd(_sockfd);
        SetEvents(EPOLLIN | EPOLLET);
    }

    void Recv() override
    {
        LOG(LogLevel::DEBUG) << "I am IOServive";

        while (true) // 因为文件描述符采用的ET模式，所以必须一次性全读完
        {
            char buff[Size];
            int n = recv(_sockfd, buff, sizeof(buff) - 1, 0);
            if (n > 0)
            {
                // 读取成功
                buff[n] = 0;
                Append(buff);
            }
            else if (n == 0) // 没有数据读了，这里全部转换到Excep里面，统一处理异常
            {
                Excep();
                return;
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    // 本轮缓冲区数据全部读完了
                    break;
                }
                else if (errno == EINTR) // 用户退出
                {
                    continue;
                }
                else
                {
                    Excep();
                    return;
                }
            }
        }

        // 下面就要使用协议去确保读到的是完整的报文数据
        std::cout << "inbuffer: " << GetInBuffer() << std::endl;
        std::string result;
        if (_onmessage)
        {
            result = _onmessage(GetInBuffer());
        }
        // 添加应答信息
        SetOutBuffer(result);
        if (!GetOutBuffer().empty())
        {
            Send();
        }
    }
    void Send() override
    {
        while (true)
        {
            // send函数是将数据写入内核的发送缓冲区
            int n = ::send(GetSockfd(), GetOutBuffer().c_str(), GetOutBuffer().size(), 0);
            if (n > 0)
            {
                // n为已经写入的n个字节
                // 存在情况：GetOutBuffer().size()>n 需要多次写入发送缓冲区
                DiscardOutBuffer(n);
            }
            else if (n == 0) // 防止outbuffer里面是空的情况
            {

                break;
            }
            else // 发送失败，判断出错类型
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) // 发送缓冲区已经满了，没空间继续写入
                {
                    // 这里就需要使用epoll，像epoll里面注册写事件
                    // 因为发送数据的时候，默认就是有空间的，如果直接注册写事件，那么就会一直触发响应，默认写事件是具备的
                    // 只有当现在的这种情况，写事件是不满足条件的时候，才需要去交给epoll去托管
                    break;
                }
                else if (errno == EINTR) // 收到信号终止
                {
                    continue;
                }
                else // 真正的发生错误
                {
                    Excep();
                    return;
                }
            }
            if(!GetOutBuffer().empty())
            { 
                // 这里就需要使用epoll，像epoll里面注册写事件

            }
            // 创建连接

            // 注册
        }
    }
    void Excep() override
    {
        // IO读取的时候，发生的异常，全部进入到Excep异常函数
    }
    void RegisterOnMeassage(func_t onmessage)
    {
        _onmessage = onmessage;
    }
    void Who()
    {
        std::cout << "i am IOService" << std::endl;
    }

    ~IOService() {}
};