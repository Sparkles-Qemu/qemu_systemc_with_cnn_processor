

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include <inttypes.h>
#include "tlm_utils/simple_target_socket.h"


#include "dma_socket_wrapper.h"

dma_socket_wrapper::dma_socket_wrapper(sc_module_name name, int _size) : sc_module(name)

{



  socket.register_b_transport(this, &dma_socket_wrapper::b_transport);
  socket.register_get_direct_mem_ptr(this, &dma_socket_wrapper::get_direct_mem_ptr); 


  size = _size;
  data_buffer = new uint8_t[size]; // This buffer will be used to store descriptor from user-app
  memset(&data_buffer[0], 0, size);


}



//TODO: in b_transport, I need to check if dma_ptr == NULL

void dma_socket_wrapper::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64    addr = trans.get_address();
	unsigned char*   ptr = trans.get_data_ptr();
	unsigned int     len = trans.get_data_length();
	unsigned char*   byt = trans.get_byte_enable_ptr();
	Descriptor *temp_descriptor;

	if (addr > sc_dt::uint64(size)) {
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		SC_REPORT_FATAL("DMA_WRAPPER", "Unsupported access\n");
		return;
	}
	if (byt != 0) {
		trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		SC_REPORT_FATAL("DMA_WRAPPER", "Unsupported access\n");
		return;
	}

	if (trans.get_command() == tlm::TLM_READ_COMMAND){
		
			memcpy(ptr, &data_buffer[addr], len);
  
  }
	else if (cmd == tlm::TLM_WRITE_COMMAND){

		memcpy(&data_buffer[addr], ptr, len);

		if( addr == size - len){
			
			//This is the last write. Reinterpret cast to Descriptor type
			temp_descriptor = reinterpret_cast<Descriptor *>(data_buffer);

			if(dma_ptr != NULL){
				//TODO: call load program function 
				//TODO: Call print descriptor function
			}else{
				
				cout<< "dma_ptr is NULL" << endl;
			}

		}

  }

	delay += LATENCY;

	trans.set_dmi_allowed(true);
	trans.set_response_status(tlm::TLM_OK_RESPONSE);
}


bool dma_socket_wrapper::get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
				tlm::tlm_dmi& dmi_data)
{
	dmi_data.allow_read_write();

	dmi_data.set_dmi_ptr( reinterpret_cast<unsigned char*>(&data_buffer[0]));
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(size - 1);
	/* Latencies are per byte.  Our latency is expressed per access,
	   which are in 32bits so dividie by 4. Is there a better way?.  */
	dmi_data.set_read_latency(LATENCY / 4);
	dmi_data.set_write_latency(LATENCY / 4);
	return true;
}

