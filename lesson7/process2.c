#include "process.h"
#define STYLE '='
const char * label="\\|/-";
void process()
{
	int cnt=0;
	char  buf[102];
	memset(buf,'\0',sizeof(buf));
	while(cnt<=101)
	{
		if(cnt==101)
		{
			break;
		}
		printf("[%-100s][%d%%][%c]\r",buf,cnt,label[cnt%4]);
		fflush(stdout);
		buf[cnt]=STYLE;
		++cnt;
		sleep(0.5);
	}
}
