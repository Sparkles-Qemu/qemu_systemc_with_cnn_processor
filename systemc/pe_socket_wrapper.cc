#define SC_INCLUDE_DYNAMIC_PROCESSES

#include <inttypes.h>

#include "tlm_utils/simple_target_socket.h"

#define MAX_WEIGHTS 3

using namespace sc_core;
using namespace std;

#include "pe_socket_wrapper.h"
#include <sys/types.h>
#include <time.h>

pe_socket_wrapper::pe_socket_wrapper(sc_module_name name)
	: sc_module(name), socket("socket")
{
	socket.register_b_transport(this, &pe_socket_wrapper::b_transport);
  message_count = 0;
}

void pe_socket_wrapper::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 addr = trans.get_address();
	unsigned char *data = trans.get_data_ptr();
	unsigned int len = trans.get_data_length();
	unsigned char *byt = trans.get_byte_enable_ptr();
	unsigned int wid = trans.get_streaming_width();
  int temp_weight = 0;

	if (byt != 0) {
		trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		return;
	}

	if (len > 4 || wid < len) {
		trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		return;
	}
  
  cout << "Addr = " << addr << endl;  
	if(cmd == tlm::TLM_WRITE_COMMAND){

    temp_weight = *(reinterpret_cast<int*>(data));
    cout << "Data = " << temp_weight << endl;
    cout << "message_count = " << message_count << endl;
    weights.push_back(temp_weight);
    message_count++;

    if(message_count == MAX_WEIGHTS) {
      if(pe_group_ptr != NULL){
        pe_group_ptr->loadWeights(weights);
        message_count = 0;
      } else{
        cout << "pe_group_ptr is null" << endl;
      }
    }
	}	
  
	trans.set_response_status(tlm::TLM_OK_RESPONSE);
}
