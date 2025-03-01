#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
    printf("First socket fd: %d\n", sockfd1);

    // 打开一个文件，占用一个文件描述符
    int fd = open("test.txt", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    printf("Opened file fd: %d\n", fd);

    close(sockfd1);

    int sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
    printf("Second socket fd: %d\n", sockfd2);

    close(fd);
    return 0;
}