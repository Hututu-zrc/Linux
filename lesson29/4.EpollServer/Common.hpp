#pragma once
#include <string>
const int gdefaultsockfd = -1;
const int gdefaultport = 8888;

#define BACKLOG 8
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
    LISTEN_ERR,
    ACCEPT_ERR,
    CONNECT_ERR,
    RECV_ERR,
    SEND_ERR,
    EPOLL_CREATE_ERR,
    EPOLL_CTL_ERR,
    EPOLL_WAIT_ERR

};

// 返回有三种可能：
// 1、out不为空，返回true 正常读取
// 2、out为空，返回true //读到空行,也就是一行里面只有"\r\n"
// 3、out为空，返回false
bool PraseOneLine(std::string &str, std::string *out, const std::string &sep)
{
    auto pos = str.find(sep);
    if (pos == std::string::npos)
        return false;
    *out = str.substr(0, pos);
    str.erase(0, pos + sep.size());
    return true;
}


//分割请求报头里面的kv
bool SplitString(std::string &header, std::string Sep, std::string *key, std::string *value)
{
    auto pos = header.find(Sep);
    if (pos == std::string::npos)
        return false;
    *key = header.substr(0, pos);
    *value = header.substr(pos + Sep.size());
    return true;
}
