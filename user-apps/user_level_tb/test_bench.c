#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

//my includes

#define SYSTEMC_DEVICE_ADDR (0xa0800000)
#define SYSTEMC_DEVICE_MMR_ADDR (0xa1000000ULL)
#define IMAGE_WIDTH 10
#define IMAGE_HEIGHT 10
#define IMAGE_SIZE IMAGE_WIDTH *IMAGE_HEIGHT
#define SMALL_RAM_SIZE IMAGE_SIZE
#define BIG_RAM_SIZE 3 * IMAGE_SIZE


int main(int argc, char *argv[])
{
	int fd, i;
	unsigned char *base_ptr, *base_ptr_mmr;
	unsigned val;
	unsigned addr, page_addr, page_offset;
	unsigned page_size=sysconf(_SC_PAGESIZE);
	int src[BIG_RAM_SIZE] = {0};
  float dst[64] = {0};
	int enable_tb, error;
	int *memory_ptr;


  float expected_output[64] = {15678, 15813, 15948, 16083, 16218, 16353, 16488, 16623, 17028, 17163, 17298    , 17433, 17568, 17703, 17838, 17973, 18378, 18513, 18648, 18783, 18918, 19053, 19188, 19323, 19728, 19863, 19998, 20133, 20268, 20403, 20538, 20673, 21078, 21213, 21348, 21483, 21618, 21753, 21888, 22023, 22428, 22563, 22698, 22833, 22968, 23103, 23238, 23373, 23778, 23913, 24048, 24183, 24318, 24453, 24588, 24723, 25128, 25263, 25398, 25533, 25668, 25803, 25938, 26073};
	
  //open virtual file to write to absolute address
	fd=open("/dev/mem",O_RDWR);
	if(fd<1) {
		perror(argv[0]);
		exit(-1);
	}



	page_addr=(SYSTEMC_DEVICE_ADDR & ~(page_size-1));
	page_offset=SYSTEMC_DEVICE_ADDR - page_addr;
	base_ptr = (unsigned char *) mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(SYSTEMC_DEVICE_ADDR & ~(page_size-1)));

	if(base_ptr == NULL) {

		printf("Error mmapping base_ptr\n");
	}

 	base_ptr_mmr = (unsigned char *) mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(SYSTEMC_DEVICE_MMR_ADDR & ~(page_size-1)));

	if (base_ptr_mmr == NULL) {

		printf("Error mmappin base_ptr_mmr\n");
	}


	for(i = 0; i < BIG_RAM_SIZE; i++) {
		src[i] = i + 1;
	}

	// Write data
	memcpy(base_ptr, src, sizeof(src));
	enable_tb = 0;

	// Enable test bench  
	memcpy(base_ptr_mmr + 8, &enable_tb,  sizeof(enable_tb));

  usleep(100); // 10 miliseconds

  // Retrieve data bac
  memcpy(dst, base_ptr + 1220, sizeof(dst));


  //print data 
  for(i = 0; i < 64; i++) {
    
    printf("value read back = %f\n",  dst[i]);
    if(dst[i] != expected_output[i])  error++;
  }

  if(error == 0)
    printf("Test bench success\n");
  else
    printf("Test bench error\n");

	return 0; 
}
