#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>

#define SYSTEMC_DEVICE_ADDR (0xa0800000ULL)

int main(int argc, char const *argv[])
{
	unsigned char* base_ptr;
	unsigned addr, page_addr, page_offset;
	unsigned page_size = sysconf(_SC_PAGESIZE);
	int fd = open("/dev/mem", O_RDWR);
	if (fd < 1)
	{
		perror(argv[0]);
		exit(-1);
	}
	page_addr = (SYSTEMC_DEVICE_ADDR & ~(page_size - 1));
	page_offset = SYSTEMC_DEVICE_ADDR - page_addr;
	base_ptr = (unsigned char*)mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (SYSTEMC_DEVICE_ADDR & ~(page_size - 1)));

	int i;
	float result[100];
	for(i = 512; i<512+100; i++)
	{
		printf("MEM[%d] = %f\n", i, ((float*)base_ptr)[i]);
	}

	printf("MEMCPY METHOD\n");
	memcpy(result, base_ptr+(512*sizeof(float)), 100*sizeof(float));

	for(i = 0; i<100; i++)
	{
		printf("RESULT[%d] = %f\n", i, result[i]);
	}

	return 0;
}
