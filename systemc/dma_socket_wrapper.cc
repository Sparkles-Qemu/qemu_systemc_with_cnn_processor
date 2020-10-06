

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include <inttypes.h>
#include "tlm_utils/simple_target_socket.h"


using namespace std;
using namespace sc_score;



#include "dma_socket_wrapper.h"

dma_socket_wrapper::dma_socket_wrapper(sc_module_name name) : sc_module(name)

{



  socket.register_b_transport(this, &dma_socket_wrapper::b_transport);
  socket.register_get_direct_mem_ptr(this, &dma_socket_wrapper::get_direct_mem_ptr); 

  //TODO: Init data buffer 

}
