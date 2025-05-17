#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string>

using std::cerr;
using std::cout;
using std::endl;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int cnt = 0;

void *routine(void *args)
{
    std::string str = static_cast<const char *>(args);
    while (true)
    {
        pthread_mutex_lock(&lock);

        pthread_cond_wait(&cond, &lock);
        cout << str << endl;
        pthread_mutex_unlock(&lock);
    }
    sleep(1);
}
int main()
{
    pthread_t td1, td2;

    pthread_create(&td1, nullptr, routine, (void *)"thread_1");
    pthread_create(&td2, nullptr, routine, (void *)"thread_2");
    while (true)
    {
        cnt++;
        if (cnt >= 10)
        {
            pthread_cond_signal(&cond);
        }
        sleep(1);
        cout << "i am dominant process" << endl;
    }

    pthread_join(td1, nullptr);
    pthread_join(td2, nullptr);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    return 0;
}