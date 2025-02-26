#include "UdpServer.hpp"
#include "Log.hpp"
#include "User.hpp"

using namespace LogModule;
using namespace UserModule;
int main()
{
    ENABLE_CONSOLE_LOG();
    std::unique_ptr<UdpServer> server = std::make_unique<UdpServer>();

    // function要求复制，unique_ptr不可以复制，所以这里使用shared_ptr
    std::shared_ptr<UserManage> um = std::make_shared<UserManage>();

    server->InitServer();

    // 这里的核心是将UserManage的AddUser和Route函数传递给UdpServer,有两种办法

    // 使用bind绑定,bind绑定um里面的成员函数
    // bind的主要作用就是绑定成员函数，然后两个占位参数知道被调用的时候，传递实际的参数
    // 当后续调用 _adduser（即 server->_adduser）时，实际上是先调用这个绑定对象，绑定对象再根据存储的信息去调用 um->AddUser。
    // 这里存在一个中间的绑定对象进行转发，增加了一层间接调用。
    // server->RegisterService(
    //     std::bind(&UserManage::AddUser, um, std::placeholders::_1),
    //     std::bind(&UserManage::Route, um, std::placeholders::_1, std::placeholders::_2));

    // 使用lambda表达式创建函数对象，将函数对象传递给UdpServer
    // Lambda 表达式本质上是编译器自动生成一个匿名类，并重载 operator()。
    // 以 [&um](const Inet_addr& addr) { um->AddUser(addr); } 为例，（个人理解，um有点像匿名类的内部成员变量了）
    // 当调用 _adduser 时，直接执行 Lambda 表达式内部定义的代码 um->AddUser(addr)，没有中间的转发层。
    server->RegisterService(
        [&um](const Inet_addr &addr)
        { um->AddUser(addr); },
        [&um](int sockfd, const std::string &msg)
        { um->Route(sockfd, msg); },
        [&um](const Inet_addr &addr)
        { um->DelUser(addr); });

    // LOG(LogLevel::DEBUG)<<"_socketfd: "<<server->GetSocketFd();;

    server->Start();

    return 0;
}