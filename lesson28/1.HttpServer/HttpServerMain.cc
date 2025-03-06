#include "HttpServer.hpp"

int main()
{
    std::unique_ptr<HttpServer> httpserver=std::make_unique<HttpServer> (8888);
    httpserver->Start();
    return 0;
}