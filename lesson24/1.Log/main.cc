#include "Log.hpp"


using namespace LogModule;
int main()
{
    ENABLE_CONSOLE_LOG();
    LOG(LogLevel::DEBUG)<<"hello zrc";
    LOG(LogLevel::DEBUG)<<"hello zrc";

    ENABLE_FILE_LOG();
    LOG(LogLevel::DEBUG)<<"hello zrc";
    LOG(LogLevel::DEBUG)<<"hello zrc";
    return 0;
}