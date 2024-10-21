#include <stdio.h>



//这里就是解open里面的标记位底层是如何实现的
//我们这里定义宏，从位图上看，看每个bite代表的时候不同的含义
//然后PrintTest里面进行if判断，注意不是elseif，是if
//每次传递的时候，就直接ONE | TWO,这样ONE和TWO上面的比特位就变成了1
//if判断就会判断到，于是可以实现多个功能
#define ONE (1<<0)
#define TWO (1<<1)
#define THREE (1<<2)
#define FOUR  (1<<3)


void PrintTest(int flag)
{
	if(flag& ONE)
	{
		printf("ONE\n");
	}	
	if(flag& TWO)
	{
		printf("TWO\n");
	}	
	if(flag& THREE)
	{
		printf("THREE\n");
	}	
	if(flag& FOUR)
	{

		printf("FOUR\n");
	}	

}
int main()
{
	PrintTest(ONE | TWO);

	return 0;
}


