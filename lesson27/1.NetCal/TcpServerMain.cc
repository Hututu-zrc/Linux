#include "TcpServer.hpp"
#include "Calculate.hpp"
#include "Prase.hpp"
#include <memory>
#include <iostream>

// 这一层就属于中间层
// 负责对于报文进行判断，传递给calculate
// 然后拿到calculate返回的数据进行封装成报文

int main()
{

    // 最上层，只负责计算就行
    std::unique_ptr<Calculator> cal = std::make_unique<Calculator>();

    // 中间层，用于计算，将每一层都分开
    //这里也是采用回调函数的方式，Prase里面内置cal->Execute函数指针类型
    //就不需要把整个类cal都融合到prase类里面，只需要把cal的某个需要的方法当作prase类的成员变量
    //然后使用回调函数的形式传入使用的函数的指针，这里不要想的太复杂
    //实际上就是，传入一个和cal_t类型一样的函数指针，这个函数指针里面参数都和cal->Execute函数一样
    //然后这个函数指针里面再去调用cal->Execute
    std::unique_ptr<Prase> prase = std::make_unique<Prase>([&cal](const Request &req) {
        return cal->Execute(req);
    });
    // LOG(LogLevel::DEBUG)<<"prase";
    // 服务器是最底层只负责IO
    std::unique_ptr<TcpServer> server = std::make_unique<TcpServer>([&prase](std::string &package) {
            return prase->PraseMessage(package);
    });
    server->InitServer();
    server->Start();
    server->Stop();
    return 0;
}