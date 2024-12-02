#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

using std::cerr;
using std::cout;
using std::endl;

void Handler(int signo)
{

}
int main()
{
    sigset_t pset;
    ::sigemptyset(&pset);
    int n=::sigpending(&pset);
    if(n<0)
    {
        cerr<<"sigpending"<<endl;
        exit(1);
    }
    while(true)
    {
        cout<<"curr pending list [" <<getpid()<<"]:";
        for(int i=1;i<=31;i++)
        {
            int ret=::sigismember(&pset,i);
            if(ret==1)
                cout<<1;
            else if(ret==0)
                cout<<0;
            else
                cerr<<"sigismember"<<endl;
        }
        cout<<endl;
        sleep(1);
    }
    return 0;
}














// void Handler(int signo)
// {
//     cout << "signo:" << signo << endl;
//     cout << "receive signo!" << endl;
// }
// int main()
// {
//     // int cnt = 10;

//     int b=10;
//     b/=0;
//     signal(SIGFPE, Handler);
//     while (true)
//     {

//     }
//     return 0;
// }

// void Handler(int signo)
// {
//     cout << "signo:" << signo << endl;
//     cout << "receive signo!" << endl;
//     exit(0);
// }
// int main()
// {
//     // int cnt = 10;

//     signal(SIGALRM, Handler);
//     while (true)
//     {
//         std::cout << "hello" << std::endl;
//         sleep(1);

//         // alarm不能放在循环里面，不然永远没有机会终止程序，
//         // 因为刚准备终止，新的alarm信号又来了
//         //  signal(2,Handler);
//     }
//     return 0;
// }