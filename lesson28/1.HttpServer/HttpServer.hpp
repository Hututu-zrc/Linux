#pragma once

#include <memory>
#include "TcpServer.hpp"
#include "HttpProtocol.hpp"

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
        std::string http_request;
        sockfd->Recv(&http_request); // 不做完整性分析，太麻烦，主要是了解http的过程即可
        HttpRequest req;
        req.Deserialize(http_request);
        req.Print();

        HttpResponse resp;
        resp.Build(req);
        std::string resp_str;
        resp.Serialize(&resp_str);
        sockfd->Send(resp_str);

        // 读取请求，对请求进行分析处理 --->文本处理
        // 1、读取请求的完整性 ---暂时不做
        // 2、完成http序列化和，http response序列化

        // // demo 1 :直接返回一个固定的内容
        // std::string status_line = "HTTP/1.1 200 OK" + Sep + BlankLine;
        // std::string body = "<!DOCTYPE html>\
        // <html>\
        // <head>\
        // <meta charset=\"UTF-8\">\
        // <title>Hello World</title>\
        // </head>\
        // <body>\
        // <h1>Hello World</h1>\
        // </body>\
        // </html>";
        // std::string httpresponse = status_line + body;
        // sockfd->Send(httpresponse);
        return true;
    }

    ~HttpServer() {}

private:
    std::unique_ptr<TcpServer> _tcsv;
};