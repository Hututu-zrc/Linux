#include <iostream>

#include "Client.hpp"

int main()
{
    Client client;
    client.OpenPipeForWrite();
    std::string message;
    while(true)
    {
        std::cout<<"Please Say#";
        getline(std::cin,message);
        client.SendPipe(message);
    }

    client.ClosePipeForWrite();
    return 0;
}