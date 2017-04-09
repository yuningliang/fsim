#include <stdio.h>

// Updated nnnnn

int foo(int x)
{
	int sum=0,i;
	for (i = 0; i<x; i++) sum += i*i;
	return sum;
}
int main(int argc, char *argv[])
{
	printf("Hello:%d\n", foo(100));
	return 0;
}

