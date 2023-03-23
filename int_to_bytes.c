#include <stdio.h>


int main()
{
	int n = 5550;
	unsigned char* a = (unsigned char*)&n;
	
	for(int i = 0; i < sizeof(int); i++)
	{
		printf("%x |", a[i]);
	}
	
	
	return 0;
}
