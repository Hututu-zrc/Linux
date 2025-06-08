#include "bq2.hpp"



BlockQueue<int> bq(10);


void Provider()
{
    // std::unique_lock<std::mutex> lock()
    bq.Enqueue(10);
}


void Consumer()
{
    std::cout<<bq.Pop()<<std::endl;
}

// void Provider()
// {
//     for (int i = 0; i < 5; ++i) {
//         bq.Enqueue(i);
//         std::cout << "Produced: " << i << std::endl;
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }
// }

// void Consumer()
// {
//     for (int i = 0; i < 5; ++i) {
//         int value = bq.Pop();
//         std::cout << "Consumed: " << value << std::endl;
//     }
// }


int main()
{
    std::thread t1(Provider);
    std::thread t2(Consumer);


    t1.join();
    t2.join();
    return 0;
}