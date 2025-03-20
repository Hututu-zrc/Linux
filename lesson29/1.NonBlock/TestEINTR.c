#include <stdio.h>
#include <unistd.h>
#include <errno.h>

ssize_t read_retry(int fd, void *buf, size_t count) {
    ssize_t n;
    while ((n = read(fd, buf, count)) == -1 && errno == EINTR) {
        // 若 read 被信号中断，重试
        continue;
    }
    return n;
}

int main() {
    char buffer[1024];
    ssize_t n = read_retry(0, buffer, sizeof(buffer));
    if (n == -1) {
        perror("Read error");
    } else {
        printf("Read %zd bytes\n", n);
    }
    return 0;
}