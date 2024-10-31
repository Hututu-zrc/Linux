#pragma once
#include <unistd.h>

#define SIZE 1024

#define FLUSH_NONE 0
#define FLUSH_LINE 1
#define FLUSH_FULL 2


struct m_file
{
	int flag;//打开的方式，行刷新或者是存满再刷新
	int fileno;//文件描述符
	char buffer[SIZE];
	int cap;//缓冲区里面的容量
	int size;//缓冲区的里面的数据大小

};

typedef struct m_file file;

file*  mfopen(const char *path, const char *mode);
size_t mfwrite(const void *ptr, size_t size, file *stream);
size_t mfread(void *ptr, size_t size, file *stream) ;


