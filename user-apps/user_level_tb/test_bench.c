#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>

//my includes

#define SYSTEMC_DEVICE_ADDR       (0xa0800000ULL)
#define SYSTEMC_DEVICE_MMR_ADDR   (0xa1000000ULL)
#define SYSTEMC_DMA_ADDR          (0xa8000000ULL)
#define SYSTEMC_PE_ADDR           (0xa9000000ULL)
#define IMAGE_WIDTH 10
#define IMAGE_HEIGHT 10
#define IMAGE_SIZE IMAGE_WIDTH *IMAGE_HEIGHT
#define SMALL_RAM_SIZE IMAGE_SIZE
#define BIG_RAM_SIZE 3 * IMAGE_SIZE
#define PE_GROUP_MAPPED_SZ        (4*3)

#define SUSPENDED 0
#define TRANSFER  1
#define TRANSFER_WITH_FORWARD 2
#define WAIT 3

struct Descriptor
{
	uint32_t next;     // index of next descriptor
	uint32_t start;    // start index in ram array
	uint32_t state;    // state of dma
	uint32_t x_count;  // number of floats to transfer/wait
	uint32_t x_modify; // number of floats between each transfer/wait
};

#define DESCRIPTOR_SZ     sizeof( struct Descriptor)
#define RAM1_DESCRIPTORS	          3
#define RAM2_DESCRIPTORS	          3
#define RAM3_DESCRIPTORS	          3
#define BRANCH0_GROUP0_DESCRIPTORS  3
#define BRANCH0_GROUP1_DESCRIPTORS  3
#define BRANCH0_GROUP2_DESCRIPTORS  3
#define BRANCH1_GROUP0_DESCRIPTORS  3
#define BRANCH1_GROUP1_DESCRIPTORS  3
#define BRANCH1_GROUP2_DESCRIPTORS  3
#define BRANCH2_GROUP0_DESCRIPTORS  3
#define BRANCH2_GROUP1_DESCRIPTORS  3
#define BRANCH2_GROUP2_DESCRIPTORS  3
#define LOOPBACK_DESCRIPTORS 3

#define BRANCH0_GROUP0_WEIGTHS      3
#define BRANCH0_GROUP1_WEIGTHS      3
#define BRANCH0_GROUP2_WEIGTHS      3
#define BRANCH1_GROUP0_WEIGTHS      3
#define BRANCH1_GROUP1_WEIGTHS      3
#define BRANCH1_GROUP2_WEIGTHS      3
#define BRANCH2_GROUP0_WEIGTHS      3
#define BRANCH2_GROUP1_WEIGTHS      3
#define BRANCH2_GROUP2_WEIGTHS      3


int main(int argc, char *argv[])
{
	int fd, i;
	unsigned char *base_ptr, *base_ptr_mmr, *base_dma_ptr, *base_pe_ptr;
	unsigned val;
	unsigned addr, page_addr, page_offset;
	unsigned page_size=sysconf(_SC_PAGESIZE);
	int src[BIG_RAM_SIZE] = {0};
	float dst[64] = {0};
	int enable_tb, enable_modules, reset_modules, error, base_dma_ptr_offset = 0, base_pe_ptr_offset = 0;
	int *memory_ptr;

	// Descriptor for source ram
	struct Descriptor desc_mm2s = {0, 0, TRANSFER, BIG_RAM_SIZE, 1};

	// Destination ram 1 descriptors
	struct Descriptor ram1_descriptors[RAM1_DESCRIPTORS] = {

		{1, 0, WAIT, 1, 1},
		{2, 0, TRANSFER, SMALL_RAM_SIZE, 1},
		{0, 0, SUSPENDED, 0, 1}};
	// Destination ram2 descriptors
	struct Descriptor ram2_descriptors[RAM2_DESCRIPTORS] = {

		{1, 0, WAIT, 1 + SMALL_RAM_SIZE, 1},
		{2, 0, TRANSFER, SMALL_RAM_SIZE, 1},
		{0, 0, SUSPENDED, 0, 1}};
	// Destination ram3 descriptors
	struct Descriptor ram3_descriptors[RAM3_DESCRIPTORS] = {
		{1, 0, WAIT, 1 + 2 * SMALL_RAM_SIZE, 1},
		{2, 0, TRANSFER, SMALL_RAM_SIZE, 1},
		{0, 0, SUSPENDED, 0, 1}};

	struct Descriptor branch0_group0_descriptors[BRANCH0_GROUP0_DESCRIPTORS] = {

		{1, 0, WAIT, BIG_RAM_SIZE, 1},
		{2, 0, TRANSFER, IMAGE_SIZE, 1},
		{2, 0, SUSPENDED, 0, 1}};

	struct Descriptor branch0_group1_descriptors[BRANCH0_GROUP1_DESCRIPTORS] = {


		{1, 0, WAIT, BIG_RAM_SIZE + 3, 1}, // 2
		{2, 10, TRANSFER, IMAGE_SIZE, 1},
		{2, 0, SUSPENDED, 0, 1}};

	struct Descriptor branch0_group2_descriptors[BRANCH0_GROUP2_DESCRIPTORS] = {

		{1, 0, WAIT, BIG_RAM_SIZE + 6, 1},
		{2, 20, TRANSFER, IMAGE_SIZE, 1},
		{2, 0, SUSPENDED, 0, 1}};

	struct Descriptor branch1_group0_descriptors[BRANCH1_GROUP0_DESCRIPTORS] = {

		{1, 0, WAIT, BIG_RAM_SIZE + 9, 1},
		{2, 0, TRANSFER, IMAGE_SIZE, 1},
		{2, 0, SUSPENDED, 0, 1}};

	struct Descriptor branch1_group1_descriptors[BRANCH1_GROUP1_DESCRIPTORS] = {

		{1, 0, WAIT, BIG_RAM_SIZE + 12, 1},
		{2, 10, TRANSFER, IMAGE_SIZE, 1},
		{2, 0, SUSPENDED, 0, 1}};

	struct Descriptor branch1_group2_descriptors[BRANCH1_GROUP2_DESCRIPTORS] = {

		{1, 0, WAIT, BIG_RAM_SIZE + 15, 1},
		{2, 20, TRANSFER, IMAGE_SIZE, 1},
		{2, 0, SUSPENDED, 0, 1}};

	struct Descriptor branch2_group0_descriptors[BRANCH2_GROUP0_DESCRIPTORS] = {

		{1, 0, WAIT, BIG_RAM_SIZE + 18, 1},
		{2, 0, TRANSFER, IMAGE_SIZE, 1},
		{2, 0, SUSPENDED, 0, 1}};

	struct Descriptor branch2_group1_descriptors[BRANCH2_GROUP1_DESCRIPTORS] = {


		{1, 0, WAIT, BIG_RAM_SIZE + 21, 1},
		{2, 10, TRANSFER, IMAGE_SIZE, 1},
		{2, 0, SUSPENDED, 0, 1}};

	struct Descriptor branch2_group2_descriptors[BRANCH2_GROUP1_DESCRIPTORS] = {

		{1, 0, WAIT, BIG_RAM_SIZE + 24, 1},
		{2, 20, TRANSFER, IMAGE_SIZE, 1},
		{2, 0, SUSPENDED, 0, 1}};

	struct Descriptor loopback_descriptors[LOOPBACK_DESCRIPTORS] = {
		{1, 0, WAIT, BIG_RAM_SIZE + 27, 1},
		{2, 512, TRANSFER, 100, 1},
		{2, 0, SUSPENDED, 0, 1}};

	int branch0_group0_weights[BRANCH0_GROUP0_WEIGTHS] = {1,2,3};
	int branch0_group1_weights[BRANCH0_GROUP1_WEIGTHS] = {4,5,6};
	int branch0_group2_weights[BRANCH0_GROUP2_WEIGTHS] = {7,8,9};

	int branch1_group0_weights[BRANCH1_GROUP0_WEIGTHS] = {1,2,3};
	int branch1_group1_weights[BRANCH1_GROUP1_WEIGTHS] = {4,5,6};
	int branch1_group2_weights[BRANCH1_GROUP2_WEIGTHS] = {7,8,9};

	int branch2_group0_weights[BRANCH2_GROUP0_WEIGTHS] = {1,2,3};
	int branch2_group1_weights[BRANCH2_GROUP1_WEIGTHS] = {4,5,6};
	int branch2_group2_weights[BRANCH2_GROUP2_WEIGTHS] = {7,8,9};


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

	base_dma_ptr = (unsigned char *) mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(SYSTEMC_DMA_ADDR & ~(page_size-1)));

	base_pe_ptr = (unsigned char *) mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(SYSTEMC_PE_ADDR & ~(page_size-1)));


	if (base_pe_ptr == NULL) {

		printf("Error mappin base_pe_ptr \n");
	}

	// Fill array with data
	for(i = 0; i < BIG_RAM_SIZE; i++) {
		src[i] = i + 1;
	}

	// Write data
	memcpy(base_ptr, src, sizeof(src));


	//reset modules
	reset_modules = 1;
	memcpy(base_ptr_mmr + 4, &reset_modules, sizeof(reset_modules));

	reset_modules = 0;                                                                                           
	memcpy(base_ptr_mmr + 4, &reset_modules, sizeof(reset_modules));


	// Send descriptor for source ram

	base_dma_ptr_offset = 0;
	memcpy(base_dma_ptr + base_dma_ptr_offset, (&desc_mm2s), DESCRIPTOR_SZ); 



	// Send Ram1 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ;
	for(i = 0; i < RAM1_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&ram1_descriptors[i]), DESCRIPTOR_SZ); 

	}


	// Send Ram2 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 2;
	for(i = 0; i < RAM2_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&ram2_descriptors[i]), DESCRIPTOR_SZ); 

	}

	// Send Ram3 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 3;
	for(i = 0; i < RAM3_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&ram3_descriptors[i]), DESCRIPTOR_SZ); 

	}

	// Send branch0_group0 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 4;
	for(i = 0; i < BRANCH0_GROUP0_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&branch0_group0_descriptors[i]), DESCRIPTOR_SZ); 

	}

	// Send branch0_group1 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 5;
	for(i = 0; i < BRANCH0_GROUP1_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&branch0_group1_descriptors[i]), DESCRIPTOR_SZ); 

	}

	// Send branch0_group2 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 6;
	for(i = 0; i < BRANCH0_GROUP2_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&branch0_group2_descriptors[i]), DESCRIPTOR_SZ); 

	}

	// Send branch1_group0 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 7;
	for(i = 0; i < BRANCH1_GROUP0_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&branch1_group0_descriptors[i]), DESCRIPTOR_SZ); 

	}

	// Send branch1_group1 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 8;
	for(i = 0; i < BRANCH1_GROUP1_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&branch1_group1_descriptors[i]), DESCRIPTOR_SZ); 

	}


	// Send branch1_group2 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 9;
	for(i = 0; i < BRANCH1_GROUP2_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&branch1_group2_descriptors[i]), DESCRIPTOR_SZ); 

	}


	// Send branch2_group0 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 10;
	for(i = 0; i < BRANCH2_GROUP0_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&branch2_group0_descriptors[i]), DESCRIPTOR_SZ); 

	}


	// Send branch2_group1 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 11;
	for(i = 0; i < BRANCH2_GROUP1_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&branch2_group1_descriptors[i]), DESCRIPTOR_SZ); 

	}

	// Send branch2_group2 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 12;
	for(i = 0; i < BRANCH2_GROUP2_DESCRIPTORS; i++){

		memcpy(base_dma_ptr + base_dma_ptr_offset, (&branch2_group2_descriptors[i]), DESCRIPTOR_SZ); 

	}

	base_dma_ptr_offset = DESCRIPTOR_SZ * 13;
	for (i = 0; i < LOOPBACK_DESCRIPTORS; i++)
	{
		memcpy(base_dma_ptr + base_dma_ptr_offset, (&loopback_descriptors[i]), DESCRIPTOR_SZ);
	}
	
	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 0;
	for(i = 0; i < BRANCH0_GROUP0_WEIGTHS; i++){

		memcpy(base_pe_ptr + base_pe_ptr_offset, &branch0_group0_weights[i], sizeof(int));

	}

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 1;
	for(i = 0; i < BRANCH0_GROUP1_WEIGTHS; i++){

		memcpy(base_pe_ptr + base_pe_ptr_offset , &branch0_group1_weights[i], sizeof(int));

	}

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 2;
	for(i = 0; i < BRANCH0_GROUP2_WEIGTHS; i++){

		memcpy(base_pe_ptr + base_pe_ptr_offset, &branch0_group2_weights[i], sizeof(int));

	}

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 3;
	for(i = 0; i < BRANCH1_GROUP0_WEIGTHS; i++){

		memcpy(base_pe_ptr + base_pe_ptr_offset, &branch1_group0_weights[i], sizeof(int));

	}

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 4;
	for(i = 0; i < BRANCH1_GROUP1_WEIGTHS; i++){

		memcpy(base_pe_ptr + base_pe_ptr_offset, &branch1_group1_weights[i], sizeof(int));

	}
	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 5;
	for(i = 0; i < BRANCH1_GROUP2_WEIGTHS; i++){

		memcpy(base_pe_ptr + base_pe_ptr_offset, &branch1_group2_weights[i], sizeof(int));

	}

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 6;
	for(i = 0; i < BRANCH2_GROUP0_WEIGTHS; i++){

		memcpy(base_pe_ptr + base_pe_ptr_offset, &branch2_group0_weights[i], sizeof(int));

	}
	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 7;
	for(i = 0; i < BRANCH2_GROUP1_WEIGTHS; i++){

		memcpy(base_pe_ptr + base_pe_ptr_offset, &branch2_group1_weights[i], sizeof(int));

	}


	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 8;
	for(i = 0; i < BRANCH2_GROUP2_WEIGTHS; i++){

		memcpy(base_pe_ptr + base_pe_ptr_offset, &branch2_group2_weights[i], sizeof(int));

	}
	
	enable_modules = 1;
	memcpy(base_ptr_mmr, &enable_modules, sizeof(enable_modules));

	sleep(5); // 5 seconds

	// Retrieve data back
  memcpy(dst, base_ptr+(512*sizeof(float)), sizeof(dst)); 

	// Disable modules                                  
	enable_modules = 0;                                                                                          
	memcpy(base_ptr_mmr, &enable_modules, sizeof(enable_modules));


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
