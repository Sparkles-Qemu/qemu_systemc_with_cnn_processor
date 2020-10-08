#include "DMA.cc"

class dma_socket_wrapper
: public sc_core::sc_module 
{
  public:
  tlm_utils::simple_target_socket<dma_socket_wrapper> socket;
  DMA *dma_ptr = NULL;
  const sc_time LATENCY;

  dma_socket_wrapper(sc_core::sc_module_name name, int _size);
  virtual void b_transport(tlm::tlm_generic_payload& trans,
                sc_time& delay);
  virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
                                          tlm::tlm_dmi& dmi_data);
  private:
  int size;
  uint8_t *data_buffer;


};

