#include "process.h"
#define STYLE '='
const char * label="\\|/-";
void Process(double total,double current)
{
	char  buf[102];
	memset(buf,'\0',sizeof(buf));
	//更新百分比
	double rate=(current/total)*100;

	//更新进度条
	for(int i=0;i<(int)rate;i++)
	{
		buf[i]=STYLE;
	}


	//打印 左对齐100个字符，不足用空格填充，打印速率，打印等待字符
	printf("[%-100s][%.1f%%][%c]\r",buf,rate,label[(int)rate%4]);
	fflush(stdout);
}
