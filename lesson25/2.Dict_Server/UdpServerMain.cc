#include "UdpServer.hpp"


int main()
{
    std::unique_ptr<UdpServer> server=std::make_unique<UdpServer>();
    server->InitServer();
    //LOG(LogLevel::DEBUG)<<"_socketfd: "<<server->GetSocketFd();;

    server->Start();
    
    return 0;
}