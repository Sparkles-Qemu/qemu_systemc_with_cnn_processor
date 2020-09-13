#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

//my includes

#define SYSTEMC_DEVICE_ADDR (0xa1000000)
#define DATA_SIZE	    (10) 

int main(int argc, char *argv[])
{
	int fd;
	void *ptr;
	unsigned val;
	unsigned addr, page_addr, page_offset;
	unsigned page_size=sysconf(_SC_PAGESIZE);
	char data1[DATA_SIZE], data2[DATA_SIZE];
	
	for(int i = 0; i < DATA_SIZE; i++){

		data1[i] = i;
		data2[i] = 1;
	}

	//open virtual file to write to absolute address
	fd=open("/dev/mem",O_RDWR);
	if(fd<1) {
		perror(argv[0]);
		exit(-1);
	}



	page_addr=(SYSTEMC_DEVICE_ADDR & ~(page_size-1));
	page_offset=SYSTEMC_DEVICE_ADDR - page_addr;

	ptr=mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(SYSTEMC_DEVICE_ADDR & ~(page_size-1)));
	if((int)ptr==-1) {
		perror(argv[0]);
		exit(-1);
	}

	memcpy(ptr+page_offset, data1, DATA_SIZE);
	memcpy(ptr+page_offset, data2, DATA_SIZE);
	
       
	return 0; 
}
