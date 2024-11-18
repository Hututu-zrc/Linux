#pragma once
#include <iostream>
#include <ctime>
#include <string>

std::string GetCurTime()
{
    time_t t=time(nullptr);
    struct tm *local_time=localtime(&t);
    char tim[1024];
    snprintf(tim,sizeof(tim),"%04d/%02d/%02d %02d:%02d:%02d",
    local_time->tm_year+1900,
    local_time->tm_mon+1,
    local_time->tm_mday,
    local_time->tm_hour,
    local_time->tm_min,
    local_time->tm_sec
    );
    return tim;
}