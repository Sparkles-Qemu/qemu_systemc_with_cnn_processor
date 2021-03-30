#include <stdio.h>
#include <string.h>
#include "user_lvl_driver.h"

#define IMAGE_WIDTH 10
#define IMAGE_HEIGHT 10
#define IMAGE_SIZE IMAGE_WIDTH *IMAGE_HEIGHT
#define SMALL_RAM_SIZE IMAGE_SIZE
#define BIG_RAM_SIZE 3 * IMAGE_SIZE

int main(int argc, char *argv[])
{
	int *cma_buf = NULL;
	int dst[64] = {0}, data_len = 0, error = 0, i = 0;
	int expected_output[64] = {15678, 15813, 15948, 16083, 16218, 16353, 16488, 16623, 17028, 17163, 17298, 17433, 17568, 17703, 17838, 17973, 18378, 18513, 18648, 18783, 18918, 19053, 19188, 19323, 19728, 19863, 19998, 20133, 20268, 20403, 20538, 20673, 21078, 21213, 21348, 21483, 21618, 21753, 21888, 22023, 22428, 22563, 22698, 22833, 22968, 23103, 23238, 23373, 23778, 23913, 24048, 24183, 24318, 24453, 24588, 24723, 25128, 25263, 25398, 25533, 25668, 25803, 25938, 26073};


	if(get_mem_ptr((uint8_t **)&cma_buf)){
		fprintf(stderr, "Could not get memory block\n");
		return 1;
	}	
	
	printf("Filling up the buffer with dummy data\n");
	for(int i = 0; i < BIG_RAM_SIZE; i++){
		*(cma_buf + i) = i + 1;
		printf("%d\n", *(cma_buf + i));
	}

	//set up irq
	if(unmask_irq()){
		fprintf(stderr, "Could not unmask irq\n");
		return 1;
	}

	data_len = BIG_RAM_SIZE * sizeof(cma_buf[0]);

	//set-up dmas
	if(program_axi_dmas(data_len)) {
		fprintf(stderr, "Could not program axi dmas\n");
		return 1;
	}

	//reset processor	
	if(reset()) {
		fprintf(stderr, "Could not reset processor\n");
		return 1;
	}

	//send descriptors 
	if(send_all_descriptors()){
		fprintf(stderr, "Could not send descriptors\n");
		return 1;
	}

	//send weights
	if(send_all_weights()){
		fprintf(stderr, "Could not send weights\n");
		return 1;
	}

	//start processor
	if(start_processor()){
		fprintf(stderr, "Could not start processor\n");
		return 1;
	}

	//wait for data ready
	if(wait_irq()) {
		fprintf(stderr, "IRQ failed\n");
		return 1;
	}


	//stop processor
	if(stop_processor()){
		fprintf(stderr, "Could not stop processor\n");
		return 1;
	}

	//put data on destination buffer
	memcpy(dst, cma_buf, sizeof(dst)); 

	//print data 
	for(i = 0; i < 64; i++) {

		printf("value read back = %d\n",  dst[i]);
		if(dst[i] != expected_output[i])  error++;
	}


	//This fails b/c were not filtering the data
	if(error == 0)
		printf("Test bench success\n");
	else
		printf("Test bench error\n");

	return 0;
}