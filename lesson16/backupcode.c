#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE * fp=fopen("test.txt","w");
	if(fp==NULL)
	{
		perror("fopen");
	}
	const char * str="hello file\n";
	fputs(str,fp);
	 fclose(fp);
	return 0;
}
