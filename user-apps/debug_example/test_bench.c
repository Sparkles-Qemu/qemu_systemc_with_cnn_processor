#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>

int main(int argc, char *argv[])
{

	for(int i = 0; i<10; i++)
	{
		printf("Hello %d\n", i);
	}

	return 0; 
}
