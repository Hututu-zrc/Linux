#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

// 定义一个包装函数来获取LWP ID，因为gettid()不是POSIX标准的一部分
pid_t gettid(void) {
    return syscall(__NR_gettid);
}

void* thread_function(void* arg) {
    pthread_t thread_id = pthread_self();
    pid_t lwp_id = gettid();

    printf("Thread ID (pthread_t): %lu, LWP ID: %d\n", (unsigned long)thread_id, lwp_id);

    // 模拟一些工作
    sleep(1);

    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // 创建第一个线程
    if (pthread_create(&thread1, NULL, thread_function, NULL) != 0) {
        perror("Failed to create thread 1");
        exit(EXIT_FAILURE);
    }

    // 创建第二个线程
    if (pthread_create(&thread2, NULL, thread_function, NULL) != 0) {
        perror("Failed to create thread 2");
        exit(EXIT_FAILURE);
    }

    // 等待两个线程完成
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}