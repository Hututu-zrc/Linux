#include <stdio.h>

int Add(int a,int b)
{
	return a+b;
}


int main()
{
	printf("hello world\n");
	int ret=Add(10,20);
	printf("%d\n",ret);
	printf("hello linux\n");
	
	return 0;
}
