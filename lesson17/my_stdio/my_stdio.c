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
	mf->cap=mf->size;
	return mf;

}
size_t mfwrite(const void *ptr, size_t size, file *stream)
{

}
size_t mfread(void *ptr, size_t size, file *stream)
{

}


int main()
{
	file * fp=mfopen("log.txt","w");
	if(fp!=NULL)
		printf("open success!,fileno:%d\n",fp->fileno);
	else
		printf("open false\n");
	return 0;
}


