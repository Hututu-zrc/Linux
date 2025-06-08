#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#define TIMEOUT 5000  // 超时时间为5秒
int main() {
    struct pollfd fds[1];
    int ret;
// 设置要监视的文件描述符和事件
    fds[0].fd = STDIN_FILENO;  // 标准输入
    fds[0].events = POLLIN;    // 监视是否有数据可读
// 调用poll函数
    ret = poll(fds, 1, TIMEOUT);
if (ret == -1) {
        perror("poll");
        return 1;
    } else if (ret == 0) {
        printf("Timeout occurred!\n");
    } else {
        // 有文件描述符状态发生变化
        if (fds[0].revents & POLLIN) {
			char buff[1024];
			int fd=read(fds[0].fd,buff,sizeof(buff)-1);
			printf("读取到的数据是: %s\r",buff);
            printf("There is data to read on stdin!\n");
        }
    }
return 0;
}
