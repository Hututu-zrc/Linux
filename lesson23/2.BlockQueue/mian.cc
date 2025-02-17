#include "BlockQueue.hpp"
#include <functional>
using namespace BlockQueueModule;
using std::cerr;
using std::cout;
using std::endl;


using func_t = std::function<void()>;

void Task()
{
    cout<<"this is a produced task"<<endl;
}
void *ConsumerTask(void *args)
{
    BlockQueue<func_t> *bq = static_cast<BlockQueue<func_t> *>(args);
    while (true)
    {
        func_t routine;
        bq->Pop(&routine);
        routine();
        //cout << "cosumer get a data: " << data << endl;
    }
}

void *ProductorTask(void *args)
{
    BlockQueue<func_t> *bq = static_cast<BlockQueue<func_t> *>(args);
    int data = 10;
    // 这里使用data产生数据，每次++
    while (true)
    {
        sleep(1);

        bq->Equeue(Task);
        //cout << "Productor process a data " << data << endl;
        data++;
    }
}


int main()
{
    // BlockQueue里面限定了条件，会自动等待
    // 队列里面没有数据的时候，生产者就会生产数据
    // 队列里面满了的时候，消费者就是消费数据
    // 即实现了互斥，也实现了同步
    BlockQueue<func_t> *bq = new BlockQueue<func_t>(5);
    pthread_t consumer, productor;
    pthread_create(&consumer, nullptr, ConsumerTask, bq);
    pthread_create(&productor, nullptr, ProductorTask, bq);

    pthread_join(consumer, nullptr);
    pthread_join(productor, nullptr);
    delete bq;
    return 0;
}