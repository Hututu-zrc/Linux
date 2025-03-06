#pragma once

const int gdefaultsockfd = -1;
const int gdefaultport=8888;

#define BACKLOG 8
#define Die(type)  \
    do             \
    {              \
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
    SEND_ERR

};

