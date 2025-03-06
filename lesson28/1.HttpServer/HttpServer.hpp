#pragma once

#include <memory>
#include "TcpServer.hpp"

using namespace TcpServerModule;
class HttpServer
{

public:
    HttpServer(int port)
        : _tcsv(std::make_unique<TcpServer>(port))
    {
    }
    void Start()
    {
        ////这里是个类成员，不能直接传入,隐含this指针，不匹配
        // 这也就是为什么传递成员函数的时候，都需要使用lambda表达式，而不是直接传递函数进去
        //_tcsv->InitTcpServer(HandlerRequest);

        _tcsv->InitTcpServer([this](SockPtr sockfd, Inet_addr client)
                             { return this->HandlerRequest(sockfd, client); });
        _tcsv->Loop();
    }
    bool HandlerRequest(SockPtr sockfd, Inet_addr client)
    {
        LOG(LogLevel::DEBUG) << "get a new client:" << client.GetIp();
        return true;
    }   

    ~HttpServer() {}

private:
    std::unique_ptr<TcpServer> _tcsv;
};