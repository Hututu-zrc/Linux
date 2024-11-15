#include <iostream>

#include"Server.hpp"


int main()
{
    Server server;
    server.OpenPipeForRead();
    std::string message;
    while(true)
    {
        int num=server.RecvPipe(&message);
        if(num==0)
        {
            break;
        }
        std::cout<<"Client Say#"<<message<<std::endl;
    }


    server.ClosePipeForRead();
    return 0;
}