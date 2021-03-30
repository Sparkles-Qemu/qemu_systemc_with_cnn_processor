#include "user_lvl_driver.h"
#include "weights.h"
#include "io_access_lib.h"
#include "descriptors.h"
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>

#define SYSTEMC_MAIN_RAM_ADDR     (0xa0800000ULL)
#define SYSTEMC_DEVICE_MMR_ADDR   (0xa1000000ULL)
#define SYSTEMC_DMA_ADDR          (0xa8000000ULL)
#define SYSTEMC_PE_ADDR           (0xa9000000ULL)
#define SYSTEMC_DEMO_DMA_ADDR 	  (0xA0010000)

// This function had to be created here so that I can places all descriptors in .h file
// Otherwise variables got redefined 
static bool send_descriptors(uint8_t *dma_ptr, const struct Descriptor *descriptors, int num_descriptors)
{
    int ret = SUCCESS;
    int i;

    if(dma_ptr == NULL || descriptors == NULL) {
        fprintf(stderr, "Cant send descriptors with null pointers\n");
        return FAILED;
    }

    for(i = 0; i < num_descriptors; i++){

		memcpy(dma_ptr, (&descriptors[i]), DESCRIPTOR_SZ); 

	}

    return ret;
}

bool program_axi_dmas(int data_len)
{
    int ret = SUCCESS;
    unsigned char *base_demo_dma_ptr = NULL;
    unsigned int phy_addr = 0;
    unsigned int dst = SYSTEMC_MAIN_RAM_ADDR; 

    if ((phy_addr = get_cma_phy_addr()) == 0) {
        fprintf(stderr, "Could not get CMA physical address\n");
        return FAILED;
    }

    //get io access
    if(ret = get_io_access(&base_demo_dma_ptr, SYSTEMC_DEMO_DMA_ADDR)){
        fprintf(stderr, "Could not get access to demo DMAs\n");
        return FAILED;
    }

    ret = program_dmas(base_demo_dma_ptr, phy_addr, dst, data_len);

    return ret;
}

bool reset(void)
{
    int ret = SUCCESS;
    unsigned char *base_mmr_ptr = NULL;

    //get io access
    if((ret = get_io_access(&base_mmr_ptr, SYSTEMC_DEVICE_MMR_ADDR))){
        fprintf(stderr, "Could not get io access for SYSTEMC_DEVICE_MMR_ADDR\n");
        return FAILED;
    }

    ret = reset_processor(base_mmr_ptr);

    return ret;
}
bool send_all_weights(void)
{
    int ret = SUCCESS;
    int base_pe_ptr_offset = 0;
    unsigned char *base_pe_ptr = NULL;

    //get io access
    if(ret = get_io_access(&base_pe_ptr, SYSTEMC_PE_ADDR)){
        fprintf(stderr, "Coudl not get io access to PEs\n");
        return FAILED;
    }

    base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 0;
    ret = send_weights(base_pe_ptr + base_pe_ptr_offset, branch0_group0_weights, BRANCH0_GROUP0_WEIGTHS);

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 1;
    ret |= send_weights(base_pe_ptr + base_pe_ptr_offset, branch0_group1_weights, BRANCH0_GROUP1_WEIGTHS);


	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 2;
    ret |= send_weights(base_pe_ptr + base_pe_ptr_offset, branch0_group2_weights, BRANCH0_GROUP2_WEIGTHS);

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 3;
    ret |= send_weights(base_pe_ptr + base_pe_ptr_offset, branch1_group0_weights, BRANCH1_GROUP0_WEIGTHS);

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 4;
    ret |= send_weights(base_pe_ptr + base_pe_ptr_offset, branch1_group1_weights, BRANCH1_GROUP1_WEIGTHS);

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 5;
    ret |= send_weights(base_pe_ptr + base_pe_ptr_offset, branch1_group2_weights, BRANCH1_GROUP2_WEIGTHS);

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 6;
    ret |= send_weights(base_pe_ptr + base_pe_ptr_offset, branch2_group0_weights, BRANCH1_GROUP2_WEIGTHS);

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 7;
    ret |= send_weights(base_pe_ptr + base_pe_ptr_offset, branch2_group1_weights, BRANCH2_GROUP1_WEIGTHS);

	base_pe_ptr_offset =  PE_GROUP_MAPPED_SZ * 8;
    ret |= send_weights(base_pe_ptr + base_pe_ptr_offset, branch2_group2_weights, BRANCH2_GROUP2_WEIGTHS);

    return ret;

}

bool send_all_descriptors(void)
{
    int ret = SUCCESS;
    int base_dma_ptr_offset = 0;
    unsigned char *base_dma_ptr = NULL;

    //get io access
    if((ret = get_io_access(&base_dma_ptr, SYSTEMC_DMA_ADDR))){
        fprintf(stderr, "Could not get access to descriptor DMAs\n");
        return FAILED;
    }

    base_dma_ptr_offset = 0;
    ret = send_descriptors(base_dma_ptr + base_dma_ptr_offset, &desc_mm2s, 1);

	// Send Ram1 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, ram1_descriptors, RAM1_DESCRIPTORS);

	// Send Ram2 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 2;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, ram2_descriptors, RAM2_DESCRIPTORS);

	// Send Ram3 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 3;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, ram3_descriptors, RAM3_DESCRIPTORS);

	// Send branch0_group0 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 4;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, branch0_group0_descriptors, BRANCH0_GROUP0_DESCRIPTORS);

	// Send branch0_group1 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 5;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, branch0_group1_descriptors, BRANCH0_GROUP1_DESCRIPTORS);

	// Send branch0_group2 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 6;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, branch0_group2_descriptors, BRANCH0_GROUP2_DESCRIPTORS);

	// Send branch1_group0 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 7;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, branch1_group0_descriptors, BRANCH1_GROUP0_DESCRIPTORS);

	// Send branch1_group1 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 8;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, branch1_group1_descriptors, BRANCH1_GROUP1_DESCRIPTORS);

	// Send branch1_group2 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 9;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, branch1_group2_descriptors, BRANCH1_GROUP2_DESCRIPTORS);

	// Send branch2_group0 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 10;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, branch2_group0_descriptors, BRANCH2_GROUP0_DESCRIPTORS);

	// Send branch2_group1 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 11;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, branch2_group1_descriptors, BRANCH2_GROUP1_DESCRIPTORS);

	// Send branch2_group2 descriptors
	base_dma_ptr_offset = DESCRIPTOR_SZ * 12;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, branch2_group2_descriptors, BRANCH2_GROUP2_DESCRIPTORS);

	base_dma_ptr_offset = DESCRIPTOR_SZ * 13;
    ret |= send_descriptors(base_dma_ptr + base_dma_ptr_offset, loopback_descriptors, LOOPBACK_DESCRIPTORS);

    return ret;
}
bool start_processor(void)
{
    int ret = SUCCESS;
    unsigned char *base_mmr_ptr = NULL;

    //get io access
    if((ret = get_io_access(&base_mmr_ptr, SYSTEMC_DEVICE_MMR_ADDR))){
        fprintf(stderr, "Could not get io access for SYSTEMC_DEVICE_MMR_ADDR\n");
        return FAILED;
    }

    ret = enable_processor(base_mmr_ptr);

    return ret;
}

bool stop_processor(void)
{
    int ret = SUCCESS;
    unsigned char *base_mmr_ptr = NULL;

    //get io access
    if((ret = get_io_access(&base_mmr_ptr, SYSTEMC_DEVICE_MMR_ADDR))){
        fprintf(stderr, "Could not get io access for SYSTEMC_DEVICE_MMR_ADDR\n");
        return FAILED;
    }

    ret = disable_processor(base_mmr_ptr);

    return ret;
} 
int get_cma_size(void) 
{
    int ret = SUCCESS;
    unsigned int buf_size = 0;
    const char *udmabuf_size = "/sys/class/u-dma-buf/udmabuf0/size";

    ret = get_cma_buf_size(udmabuf_size, &buf_size);

    return ret == SUCCESS ? (int)buf_size : 0;
}
uint32_t get_cma_phy_addr(void)
{
    int ret = SUCCESS;
    unsigned int phy_addr = 0;
    const char *dma_phy_addr = "/sys/class/u-dma-buf/udmabuf0/phys_addr";

    ret = get_cma_addr(dma_phy_addr, &phy_addr);

    return ret == SUCCESS ? phy_addr : 0;
}

bool get_mem_ptr(uint8_t **buf_ptr)
{
    int ret = SUCCESS;
    const char *dev_udmabuf = "/dev/udmabuf0";

    //get buf size 
    int buf_size = get_cma_size();
    if( buf_size == 0){
        fprintf(stderr, "Could not get cma size\n");
        return FAILED;
    }

    ret = get_cma_ptr(dev_udmabuf, buf_ptr, buf_size);
    return ret;
}
bool unmask_irq(void)
{
    int uio_fd;
    int ret = SUCCESS;
    const char *uio_name = "/dev/uio2";
    uint32_t info = 1; /* unmask */

    //setup and unmask irq
    if((ret  = setup_irq(uio_name, &uio_fd))) {
        fprintf(stderr, "Could not get uio_fd in unmask_irq()\n");
        return FAILED;
    }

    //unmask irq
	ssize_t nb = write(uio_fd, &info, sizeof(info));
	if (nb != (ssize_t)sizeof(info)) {
        fprintf(stderr, "Could not unmask irq\n");
		close(uio_fd);
        return FAILED;
	}

    close(uio_fd);
    return ret;
}

bool wait_irq(void)
{
    int uio_fd;
    int ret = SUCCESS;
    uint32_t info;
    const char *uio_name = "/dev/uio2";
    ssize_t nb;

    //setup and unmask irq
    if((ret  = setup_irq(uio_name, &uio_fd))){
        fprintf(stderr, "Could not get uio_fd in wait_irq()\n");
        return FAILED;
    }

    struct pollfd uio_fds = {
            .fd = uio_fd,
            .events = POLLIN,
    };

    ret = poll(&uio_fds, 1, -1);
    if (ret >= 1) {
        nb = read(uio_fd, &info, sizeof(info));
        if (nb == (ssize_t)sizeof(info)) {
            ret = SUCCESS;
            close(uio_fd);
        }
    } else {
        fprintf(stderr, "IRQ poll falied\n");
        ret = FAILED;
        close(uio_fd);
    }

    return ret;
}