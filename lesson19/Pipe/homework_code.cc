#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using std::cout;
using std::endl;
using std::cerr;

int main()
{
    int fd[2];
    int n=::pipe(fd);
    if(n<0)
    {
        cerr<<"pipe"<<endl;
        exit(1);
    }
    int id=::fork();
    if(id<0)
    {
        cerr<<"fork"<<endl;
        exit(2);
    }
    if(id==0)
    {
        ::close(fd[1]);
        char buff[1024];
        ::read(fd[0],buff,sizeof(buff)/sizeof(char));
        cout<<buff<<endl;
        exit(0);
    }

    ::close(fd[0]);
    const char *buff="i am father";
    ::write(fd[1],buff,strlen(buff));
    int ret=wait(nullptr);
    if(ret<0)
    {
        cerr<<"wait"<<endl;
        exit(2);
    }
    return 0;
}