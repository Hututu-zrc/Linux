#include <iostream>
#include <unistd.h>
#include "RingBuff.hpp"
using std::cerr;
using std::cout;
using std::endl;

using namespace RingBuffModule;

void *ConsumerTask(void *args)
{
    RingBuff<int> *rf=static_cast<RingBuff<int>*>(args);
    int data;
    while(true)
    {
        sleep(1);
        rf->Pop(&data);

        cout<<"consumer get a data: "<<data<<endl;
    }
    
}

void *ProductorTask(void *args)
{
    RingBuff<int> *rf=static_cast<RingBuff<int>*>(args);
    int data=10;
    while(true)
    {
        // cout<<"productor"<<endl;
        //sleep(1);
        rf->Equeue(data);
        cout<<"productor process a data: "<<data<<endl;
        data++;
    }
}


int main()
{
   
    RingBuffModule::RingBuff<int> *rf=new RingBuffModule::RingBuff<int>();
    pthread_t c1,c2,c3,p1,p2;
    pthread_create(&c1, nullptr, ConsumerTask, rf);
    pthread_create(&c2, nullptr, ConsumerTask, rf);
    pthread_create(&c3, nullptr, ConsumerTask, rf);
    pthread_create(&p1, nullptr, ProductorTask, rf);
    pthread_create(&p2, nullptr, ProductorTask, rf);

    pthread_join(c1, nullptr);
    pthread_join(c2, nullptr);
    pthread_join(c3, nullptr);
    pthread_join(p1, nullptr);
    pthread_join(p2, nullptr);
    delete rf;
    return 0;
}

