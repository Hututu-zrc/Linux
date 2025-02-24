#include "my_string.h"

int  my_strlen (const char *s)
{
	const char * pos =s;
	while(*pos!='\0')	{ ++pos;}
	int len=pos-s;
	return len;
}
