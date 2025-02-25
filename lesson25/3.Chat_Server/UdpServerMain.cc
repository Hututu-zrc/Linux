#include "UdpServer.hpp"
#include "Log.hpp"
#include "User.hpp"


using namespace LogModule;
using namespace UserModule;
int main()
{
    ENABLE_CONSOLE_LOG();
    std::unique_ptr<UdpServer> server=std::make_unique<UdpServer>();

    //function要求复制，unique_ptr不可以复制，所以这里使用shared_ptr
    std::shared_ptr<UserManage> um=std::make_shared<UserManage>();
    
    server->InitServer();

    //这里的核心是将UserManage的AddUser和Route函数传递给UdpServer,有两种办法
    
    //使用bind绑定,bind绑定um里面的成员函数
    server->RegisterService(
            std::bind(&UserManage::AddUser,um,std::placeholders::_1),
            std::bind(&UserManage::Route,um,std::placeholders::_1,std::placeholders::_2)
    );

    //使用lambda表达式创建函数对象，将函数对象传递给UdpServer
    
    //LOG(LogLevel::DEBUG)<<"_socketfd: "<<server->GetSocketFd();;

    server->Start();
    
    return 0;
}