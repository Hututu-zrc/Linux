#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using std::cerr;
using std::cout;
using std::endl;

int Orphan_Process()
{
    pid_t id = ::fork();
    if (id < 0)
    {
        cerr << "fork" << endl;
        return 1;
    }
    else if (id == 0)
    {
        cout << "Orphan_Process" << endl;
        sleep(10);
    }
    exit(0);

    return 0;
}
int Zombie_Process()
{
    pid_t id = ::fork();
    if (id < 0)
    {
        cerr << "fork" << endl;
        return 1;
    }
    else if (id == 0)
    {
        cout << "Zobie_Process" << endl;
        exit(0);
    }
    sleep(20);

    return 0;
}
int main()
{
    Orphan_Process();
    return 0;
}