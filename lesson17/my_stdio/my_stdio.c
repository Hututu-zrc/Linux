#include "my_stdio.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


file* mfopen(const char *path, const char *mode)
{
	int fd=-1;
	if(strcmp(mode,"r")==0)
	{
		fd=open(path,O_RDONLY);	
	}
	else if(strcmp(mode,"w")==0)
	{
		fd=open(path,O_WRONLY | O_CREAT | O_TRUNC,0666);
	}
	else if( strcmp(mode,"a")==0)
	{
		fd=open(path,O_WRONLY | O_CREAT | O_APPEND,0666);
	}
	if(fd==-1)
	{
		close(fd);
		return NULL;
	}
	file * mf=(file *)malloc(sizeof(file)*1);
	if(mf==NULL)
		perror("malloc");
	mf->fileno=fd;
	mf->flag=FLUSH_LINE;
	mf->size=0;
	mf->cap=SIZE;
	return mf;

}
size_t mfwrite(const void *ptr, size_t size, file *stream)
{
	//由于存在文件缓冲区，所以我们先将文件添加在缓冲区的末尾
	memcpy(stream->buffer+stream->size,ptr,size);
	//更新stream里面的size
	stream->size+=size;
	if(stream->size>0 && stream->flag==FLUSH_LINE && stream->buffer[size-1]=='\n')
	{
		mfflush(stream);	
	}
	return size;
}

void mfflush(file*stream)
{

	if(stream->size>0)//缓冲区有内容
	{
		//将缓冲区里面的内容刷新到目标的fileno里面
		write(stream->fileno,stream->buffer,strlen(stream->buffer));
		stream->size=0;//刷新完了以后size为0
	}
}
size_t mfread(void *ptr, size_t size, file *stream);

void mfclose(file *stream)
{
	if(stream->size>0)
	{
		mfflush(stream);
	}
	close(stream->fileno);
}
int main()
{
	file * fp=mfopen("log.txt","w");
	if(fp==NULL)
		perror("mfopen");
	char str[]="abcde\n";
	mfwrite(str,strlen(str),fp);
	mfclose(fp);
	return 0;
}


