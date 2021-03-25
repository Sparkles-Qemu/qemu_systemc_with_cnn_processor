#ifndef USER_LVL_DRIVER
#define USER_LVL_DRIVER 

#include <stdbool.h>

bool setup(void);
bool program_axi_dmas(void);
bool reset_processor(void);
bool send_all_weights(void);
bool send_all_descriptors(void)
bool start_processor(void);
bool stop_processor(void); 
int get_cma_size(void);
unsigned int get_cma_phy_addr(void);
bool get_mem_ptr(unsigned char **buf_ptr);
bool unmask_irq(void);
bool wait_irq(void);
#endif //USER_LVL_DRIVER