#include <stdio.h>
#include <string.h>
#include "process.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

const int base=20;
double total =2048.0;
double once=0.1;
typedef void(*flush_t)(double total ,double current);

/*	
	实现进度条打印的步骤：
	1、随机下载量
	2、实现进度条的打印
*/
void download(flush_t f)
{
	double current=0.0;
	while(current<total)
	{
		int r= rand()%base+1;//随机下载量
		double speed=r*once;//换成小数
		current+=speed;
		if(current>=total)
		{
			current=total;
		}
		usleep(10000);
		
	
		f(total,current);	//打印进度条
	}
	//printf("\n");
}


int main()
{

	srand(time(NULL));
	download(Process);
	return 0;
}
