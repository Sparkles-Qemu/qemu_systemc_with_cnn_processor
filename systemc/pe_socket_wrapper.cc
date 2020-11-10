#define SC_INCLUDE_DYNAMIC_PROCESSES

#include <inttypes.h>

#include "tlm_utils/simple_target_socket.h"

using namespace sc_core;
using namespace std;

#include "pe_socket_wrapper.h"
#include <sys/types.h>
#include <time.h>

pe_socket_wrapper::pe_socket_wrapper(sc_module_name name)
	: sc_module(name), socket("socket")
{
	socket.register_b_transport(this, &pe_socket_wrapper::b_transport);
}

void pe_socket_wrapper::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 addr = trans.get_address();
	unsigned char *data = trans.get_data_ptr();
	unsigned int len = trans.get_data_length();
	unsigned char *byt = trans.get_byte_enable_ptr();
	unsigned int wid = trans.get_streaming_width();

	if (byt != 0) {
		trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		return;
	}

	if (len > 4 || wid < len) {
		trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		return;
	}
	
	if(cmd == tlm::TLM_WRITE_COMMAND){

	}	

}
