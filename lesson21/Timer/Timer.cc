#include <iostream>
#include <vector>
#include <functional>
#include <unistd.h>
#include <signal.h>

using std::cerr;
using std::cout;
using std::endl;

int gcount = 0;
std::vector<std::function<void()>> gfuncs;
//将这里的信号换成硬件中断，就是OS的运行逻辑
//OS就是一个循环，然后不停接收中断，执行中断的命令
void Handler(int signo)
{
    for (auto f : gfuncs)
    {
        f();
    }
    cout << "gcount:" << gcount++ << endl;
    alarm(1);
    cout<<endl;
}

int main()
{
    gfuncs.push_back([]()
                     { cout << "this is a download task!" << endl; });
    gfuncs.push_back([]()
                     { cout << "this is a upload task!" << endl; });
    gfuncs.push_back([]()
                     { cout << "this is a log task!" << endl; });

    alarm(1); // 这里是一次性的闹钟，用了一次之后就没了
    signal(SIGALRM, Handler);
    while (true)
    {

        pause();//用来等待信号的到来
        cout<<"signal is coming!"<<endl;
    }
    return 0;
}