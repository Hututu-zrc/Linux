#include "TcpServer.hpp"
#include <memory>
#include <iostream>
int main()
{
    std::unique_ptr<TcpServer> server=std::make_unique<TcpServer>();
    server->InitServer();
    server->Start();
    return 0;
}