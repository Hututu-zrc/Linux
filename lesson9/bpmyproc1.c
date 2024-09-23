#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int main()
{
	chdir("/home/ZRC/linux");
	FILE * file=fopen("test.txt","w");
	fprintf(file,"%s","zhuruncai");
	fclose(file);
//	while(1)
//	{
//
//		printf("hello linux\n");
//		sleep(1);
//	}
	return 0;
}
