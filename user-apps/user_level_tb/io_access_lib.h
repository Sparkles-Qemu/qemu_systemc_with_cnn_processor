#ifndef IO_ACCESS
#define IO_ACCESS

#include <stdbool.h>
#include <stdint.h>

#define SUCCESS     (0)
#define FAILED      (1)

// Descript states states
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
#define PE_GROUP_MAPPED_SZ        (4*3)


bool program_dmas(unsigned char *dma_ptr, unsigned int src, unsigned int dst, int len);
bool send_weights(unsigned char *pe_ptr, const int *weights, int num_weights);
bool send_descriptors(unsigned char *dma_ptr, const int *descriptors, int num_descriptors);
bool reset_processor(unsigned char *base_mmr);
bool enable_processor(unsigned char *base_mmr);
bool disable_processor(unsigned char *base_mmr);
bool setup_irq(const char *uio_name, int *uio_fd);
bool get_cma_addr(const char *udmabuf_phy_addr, unsigned int *phy_addr);
bool get_cma_buf_size(const char *udmabuf, unsigned int *size);
bool get_cma_ptr(const char *dev_udmabuf, unsigned char **buf_ptr);
bool get_io_access(unsigned char **base_ptr, int device_addr);

#endif //IO_ACCESS