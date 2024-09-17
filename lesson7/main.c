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

void download(flush_t f)
{
	double current=0.0;
	while(current<total)
	{
		int r= rand()%base+1;
		double speed=r*once;
		current+=speed;
		if(current>=total)
		{
			current=total;
		}
		usleep(10000);
		
	
		f(total,current);	
	}
	//printf("\n");
}


int main()
{

	srand(time(NULL));
	download(Process);
	return 0;
}
