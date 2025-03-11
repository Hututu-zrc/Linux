#include "HttpServer.hpp"

void Login(HttpRequest &req, HttpResponse &resp)
{
    // 根据req，动态构建resp
    LOG(LogLevel::DEBUG) << req.GetPath() << "," << req.GetArgs();
    // 1、解析参数
    // 2、数据库核验用户是否合法
    // 3、返回响应报文
    std::string body =req.GetContent("wwwroot/success.html");
    resp.SetCode(200);

    resp.SetHeader("Content-Length", std::to_string(body.size()));
    resp.SetHeader("Content-Type", "text/html");
    resp.Setbody(body);
}
int main()
{
    std::unique_ptr<HttpServer> httpserver = std::make_unique<HttpServer>(8888);
    httpserver->Register("/login", Login);
    httpserver->Start();
    return 0;
}