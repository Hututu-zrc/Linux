#pragma once
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <memory>
#include<functional>

#include "Socket.hpp"
#include "Log.hpp"
#include "Common.hpp"
#include "InetAddr.hpp"

namespace TcpServerModule
{
    using namespace SocketModule;
    using namespace LogModule;
    //这个函数就是为了把数据传递到上层去，让上层的去解析参数，返回数据
    using tcphandler_t = std::function<bool(SockPtr,Inet_addr)>;

    class TcpServer
    {

    public:
        TcpServer(int port)
            : _port(port),
            _isrunning(false),
              _socket(std::make_unique<TcpSocket>())
        {

            _socket->BulidTcpServerMethod(port);
        }
        //假设我们把tcphandler_t放到构造函数当中，那么上层构造TcpServer的时候就需要传入回调函数指针
        //这不太方便，所以着初始化函数诞生了

        //注册处理函数
        void InitTcpServer(tcphandler_t handler)
        {
            _handler=handler;
        }

        //非常重要：这里为什么没有出现粘包问题
        //主要原因就是基于http服务的请求-响应服务
        //http每次发送一个请求，等待服务器响应了之后才会发送第二个请求
        //所以，我们每次都只是接收一个http报文，不会出现粘包问题，只会有可能出现分包问题
        //这里采取的是短连接的方式，每次处理完一个http报文就直接关闭掉，然后重新连接
        void Loop()
        {
            _isrunning = true;
            while (true)
            {
                // 获取accept
                Inet_addr Client;
                SockPtr sockfd = _socket->AcceptOrDie(&Client);
                if (sockfd == nullptr)
                {
                    continue;
                }

                // 实现调用IO的接口
                pid_t pid=fork();
                if (pid == 0)
                {
                    //子进程会拷贝父进程的文件描述符表，这个文件描述符在父进程有用，子进程用不到
                    //关闭多余的文件描述符
                    _socket->Close();
                    //处理方法处理以后，返回信息
                    _handler(sockfd,Client);
                    exit(0);

                }
                sockfd->Close();
                ::waitpid(pid,nullptr,0);
                
            }
            _isrunning=false;
        }
        ~TcpServer() {}

    private:
        bool _isrunning;
        int _port;
        std::unique_ptr<Socket> _socket;
        
        tcphandler_t _handler;//处理函数
    };
}