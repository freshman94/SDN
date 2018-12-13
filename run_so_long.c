#include <stdio.h>
#include <unistd.h>

const int MAX = 50000;

int main()
{
	int i, j, k;
	for (i = 0; i < MAX; i++)
		for (j = 0; j < MAX; j++)
			k = i * j;
	return 0;
}