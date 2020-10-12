#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

//my includes

#define SYSTEMC_DEVICE_ADDR (0xa0900000ULL)

#define SUSPENDED 0
#define TRANSFER  1
#define TRANSFER_WITH_FORWARD 2
#define WAIT 3

#define IMAGE_WIDTH 10
#define IMAGE_HEIGHT 10
#define IMAGE_SIZE IMAGE_WIDTH *IMAGE_HEIGHT
#define SMALL_RAM_SIZE IMAGE_SIZE
#define BIG_RAM_SIZE 3 * IMAGE_SIZE

struct Descriptor
{
  unsigned int next;     // index of next descriptor
  unsigned int start;    // start index in ram array
  unsigned int state;    // state of dma
  unsigned int x_count;  // number of floats to transfer/wait
  unsigned int x_modify; // number of floats between each transfer/wait
};

int main(int argc, char *argv[])
{
	int fd;
	unsigned char  *base_ptr, *dma_ptr, char_test[4] = {0, 1, 2, 3};
	unsigned addr, page_addr, page_offset;
	unsigned page_size=sysconf(_SC_PAGESIZE);

  //Test descriptor
  struct  Descriptor desc_mm2s = {0, 0, TRANSFER, BIG_RAM_SIZE, 1};
  dma_ptr = (unsigned char*)(&desc_mm2s);

	//open virtual file to write to absolute address
	fd=open("/dev/mem",O_RDWR);
	if(fd<1) {
		perror(argv[0]);
		exit(-1);
	}



	page_addr=(SYSTEMC_DEVICE_ADDR & ~(page_size-1));
	page_offset=SYSTEMC_DEVICE_ADDR - page_addr;
	base_ptr = (unsigned char *)mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(SYSTEMC_DEVICE_ADDR & ~(page_size-1)));
  

  if(base_ptr == NULL){

    printf("Error mapping base_ptr\n");
  }

	// Write data
	memcpy(base_ptr, char_test,  sizeof(char_test));
	return 0; 
}
