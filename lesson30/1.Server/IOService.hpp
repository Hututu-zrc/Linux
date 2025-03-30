#pragma once
#include <iostream>
#include <memory>
#include <sys/epoll.h>
#include "Socket.hpp"
#include "Reactor.hpp"

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

        // 这两个是设置Connection里面的属性
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
            else if (n == 0) // 对方关闭连接，这里全部转换到Excep里面，统一处理异常
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
                else if (errno == EINTR) // 此时被信号中断
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
        LOG(LogLevel::DEBUG) << "result: " << result;
        // 添加应答信息
        SetOutBuffer(result);
        if (!GetOutBuffer().empty())
        {
            // 方案1：直接send
            Send();
            // 方案2：打开对写事件的关注
            // GetEpollOwner()->ModifyConnection(_sockfd, true, true);
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

            // 这里的if判断就是从我们自己的缓冲区写入到内核缓冲区的过程当中，内核缓冲区写满了
            // 这里时候outbuffer里面就还有数据，这个时候就注册写事件的读取
            if (!GetOutBuffer().empty())
            {

                // 这里不应该这样写，应该在reactor里面去修改conn
                //  SetEvents(EPOLLIN |EPOLLOUT|EPOLLET);
                GetEpollOwner()->ModifyConnection(_sockfd, true, true);
            }
            else
            {
                // 当前面的outbuffer数据全部写入内核缓冲区的时候，就需要修改对于写事件的读取
                // 因为epoll里面读事件属于常驻，写事件是按需读取，这里就要把之前的写事件关闭掉
                GetEpollOwner()->ModifyConnection(_sockfd, true, false);
            }
        }
    }
    void Excep() override
    {
        // IO读取的时候，发生的异常，全部进入到Excep异常函数
        // 异常函数的主要功能就是写入日志，差错处理，关闭连接，reactor异常connection，移出对于fd的事件的关心
        LOG(LogLevel::INFO) << "客户端连接结束，异常处理: " << _sockfd;
        GetEpollOwner()->DeleteConnection(_sockfd);
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