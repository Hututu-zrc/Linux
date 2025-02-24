#pragma once
#include <iostream>
#include <string>

const std::string SEP = ":";

#define Die(type)   \
    do              \
    {               \
        exit(type); \
    } while (0);
#define CONV(type) (struct sockaddr *)(type)

enum Err
{
    USAGE_ERR = 1,
    SOCKET_ERR,
    BIND_ERR,
    RECVFROM_ERR,
    SENDTO_ERR

};

size_t SplitString(const std::string &str)
{
    int pos = str.find(SEP);
    if (pos == std::string ::npos)
    {
        return std::string ::npos;
    }
    return pos;
}
