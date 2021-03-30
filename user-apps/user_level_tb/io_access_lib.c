#include "io_access_lib.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define DMAS_OFFSET 0x100
#define DMA_DST_OFFSET 0
#define DMA_SRC_OFFSET 4
#define DMA_LEN_OFFSET 8
#define DMA_CTR_FLAG_OFFSET 12

#define PROCESSOR_RESET_OFFSET 4
#define PROCESSOR_ENABLE_OFFSET 0

#define OUPUT_FEATURE_MAP_OFFSET (512*sizeof(int))
#define OUPUT_FEATURE_MAP_SZ 64

bool program_dmas(uint8_t *dma_ptr, uint32_t src, uint32_t dst, int len)
{
    int ret = SUCCESS;
    int ctr_flag = 1;

    unsigned int src2 = dst + OUPUT_FEATURE_MAP_OFFSET;
    unsigned int dst2 = src;
    int len2 = OUPUT_FEATURE_MAP_SZ;


    if(dma_ptr == NULL){
        fprintf(stderr, "DMA pointer is null\n");
        return FAILED;
    }

    //setup but dont enable dma2
    memcpy(dma_ptr + DMAS_OFFSET + DMA_DST_OFFSET, &dst2, sizeof(dst2));	
	memcpy(dma_ptr + DMAS_OFFSET + DMA_SRC_OFFSET, &src2, sizeof(src2));
	memcpy(dma_ptr + DMAS_OFFSET + DMA_LEN_OFFSET, &len2, sizeof(len2));

    //setup and enable dma1
    memcpy(dma_ptr + DMA_DST_OFFSET, &dst, sizeof(dst));	
	memcpy(dma_ptr + DMA_SRC_OFFSET, &src, sizeof(src));
	memcpy(dma_ptr + DMA_LEN_OFFSET, &len, sizeof(len));
	memcpy(dma_ptr + DMA_CTR_FLAG_OFFSET, &ctr_flag, sizeof(ctr_flag));

    return ret;
}

bool send_weights(uint8_t *pe_ptr, const int *weights, int num_weights)
{
    int ret = SUCCESS;
    int i;
    if(pe_ptr == NULL || weights == NULL) {
        fprintf(stderr, "Cannot send weights with null pointers\n");
        return FAILED;
    }

    for(i = 0; i < num_weights; i++){

		memcpy(pe_ptr, &weights[i], sizeof(int));
	}

    return SUCCESS;
}



bool reset_processor(uint8_t *base_mmr)
{
    int ret = SUCCESS;
    int reset_modules;


    if(base_mmr == NULL){
        fprintf(stderr, "base_mmr is null. call get_io_access() to get a valid pointer\n");
        return FAILED;
    }

    //reset modules
	reset_modules = 1;
	memcpy(base_mmr + PROCESSOR_RESET_OFFSET, &reset_modules, sizeof(reset_modules));

	reset_modules = 0;                                                                                           
	memcpy(base_mmr + PROCESSOR_RESET_OFFSET, &reset_modules, sizeof(reset_modules));

    return ret;
}


bool enable_processor(uint8_t *base_mmr)
{
    int ret = SUCCESS;
    int enable_modules;

    if(base_mmr == NULL){
        fprintf(stderr, "base_mmr is null. call get_io_access() to get a valid pointer\n");
        return FAILED;
    }

    enable_modules = 1;
	memcpy(base_mmr + PROCESSOR_ENABLE_OFFSET, &enable_modules, sizeof(enable_modules));

    return ret;
}

bool disable_processor(uint8_t *base_mmr)
{
    int ret = SUCCESS;
    int enable_modules;

    if(base_mmr == NULL){
        fprintf(stderr, "base_mmr is null. call get_io_access() to get a valid pointer\n");
        return FAILED;
    }

    enable_modules = 0;
	memcpy(base_mmr + PROCESSOR_ENABLE_OFFSET, &enable_modules, sizeof(enable_modules));

    return ret;
}

bool setup_irq(const char *uio_name, int *uio_fd)
{
    int ret = SUCCESS;

    //open up uio device 
	*(uio_fd) = open(uio_name, O_RDWR);
	if (*(uio_fd) < 0) {
		fprintf(stderr, "Could not open %s\n", uio_name);
        return FAILED;
	}

    return SUCCESS;
}

bool get_cma_addr(const char *udmabuf_phy_addr, uint32_t *phy_addr)
{
    int ret = SUCCESS;
    int fd;

    unsigned char attr[1024];

    //extract size
	if ((fd  = open(udmabuf_phy_addr, O_RDONLY)) != -1) {
		read(fd, attr, 1024);
		sscanf(attr, "%x", phy_addr);
		close(fd);
	} else {
        fprintf(stderr, "Could not get cma_buf\n");
        ret = FAILED;
    }

    return ret;
}
bool get_cma_buf_size(const char *udmabuf, uint32_t *size)
{
    int ret = SUCCESS;
    int fd;

    unsigned char attr[1024];

    //extract size
	if ((fd  = open(udmabuf, O_RDONLY)) != -1) {
		read(fd, attr, 1024);
		sscanf(attr, "%d", size);
		close(fd);
	} else { 
       fprintf(stderr, "Could not get cma size\n");
       ret = FAILED; 
    }

    return ret;
}

bool get_cma_ptr(const char *dev_udmabuf, uint8_t **buf_ptr, int buf_size)
{
    int ret = SUCCESS;
    int fd_dmabuf; 
    if ((fd_dmabuf = open(dev_udmabuf, O_RDWR))  == -1) {

        fprintf(stderr, "Could not open %s\n", dev_udmabuf);
        close(fd_dmabuf);
        return FAILED;
    }

    *(buf_ptr) = (uint8_t *)mmap(NULL, buf_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_dmabuf, 0);

    if(*(buf_ptr) == NULL) {

        fprintf(stderr, "Could not mmap cma pointer\n");
        return FAILED;
    }

    return ret;
}

bool get_io_access(uint8_t **base_ptr, uint32_t device_addr)
{
    int ret = SUCCESS;
    int fd;
    unsigned page_size=sysconf(_SC_PAGESIZE);

    //open virtual file to mmap to physical address
	fd=open("/dev/mem",O_RDWR);

	if(fd<1) {
		fprintf(stderr, "Could not open /dev/mem/\n");
        return FAILED;
	}


    *(base_ptr) = (unsigned char *)mmap(NULL, page_size, PROT_READ|PROT_WRITE,MAP_SHARED, fd, device_addr);

    if (*(base_ptr) == NULL){
        fprintf(stderr, "Could not mmap to addres=0x%x\n", device_addr);
        ret = FAILED;
        close(fd);
    }

    close(fd);
    return ret;
}

