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