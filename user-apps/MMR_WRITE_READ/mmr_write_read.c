#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

//my includes

#define SYSTEMC_DEVICE_ADDR (0xa1000000ULL)
#define SYSTEMC_DEVICE_ADDR (0xa1000000ULL)


int main(int argc, char *argv[])
{
	int fd;
	unsigned char  *base_ptr;
	unsigned addr, page_addr, page_offset;
	unsigned page_size=sysconf(_SC_PAGESIZE);
	int enable_tb;



	//open virtual file to write to absolute address
	fd=open("/dev/mem",O_RDWR);
	if(fd<1) {
		perror(argv[0]);
		exit(-1);
	}



	page_addr=(SYSTEMC_DEVICE_ADDR & ~(page_size-1));
	page_offset=SYSTEMC_DEVICE_ADDR - page_addr;
	base_ptr = (unsigned char *)mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(SYSTEMC_DEVICE_ADDR & ~(page_size-1)));


	// Write data
  enable_tb = 0;
	memcpy(base_ptr + 8, &enable_tb,  sizeof(enable_tb));
//
// enable_tb = 0;
// memcpy(base_ptr, &enable_tb,  sizeof(enable_tb));
	return 0; 
}
