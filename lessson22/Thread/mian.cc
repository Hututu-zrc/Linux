#include "Thread.hpp"

using namespace ThreadModule;
using std::cerr;
using std::cout;
using std::endl;
void Run()
{
    while (true)
    {
        cout << "hello world" << endl;
        sleep(1);
    }
}
int main()
{
    Thread td(Run);
    td.Start();
    cout << td.GetName() << endl;
    if (td.Join())
        cout << "pthread_join is success!" << endl;
    else
        cerr << "pthread_join is success!" << endl;
        return 0;
}