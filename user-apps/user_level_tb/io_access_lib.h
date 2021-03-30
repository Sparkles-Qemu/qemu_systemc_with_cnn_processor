#ifndef IO_ACCESS_H
#define IO_ACCESS_H

#include <stdbool.h>
#include <stdint.h>


#define SUCCESS     (0)
#define FAILED      (1)


bool program_dmas(uint8_t *dma_ptr, uint32_t src, uint32_t dst, int len);
bool send_weights(uint8_t *pe_ptr, const int *weights, int num_weights);
bool reset_processor(uint8_t *base_mmr);
bool enable_processor(uint8_t *base_mmr);
bool disable_processor(uint8_t *base_mmr);
bool setup_irq(const char *uio_name, int *uio_fd);
bool get_cma_addr(const char *udmabuf_phy_addr, uint32_t *phy_addr);
bool get_cma_buf_size(const char *udmabuf, uint32_t *size);
bool get_cma_ptr(const char *dev_udmabuf, uint8_t **buf_ptr, int buf_size);
bool get_io_access(uint8_t **base_ptr, uint32_t device_addr);

#endif //IO_ACCESS