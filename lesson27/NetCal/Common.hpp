#pragma once

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
    RECVFROM_ERR,
    SENDTO_ERR

};
