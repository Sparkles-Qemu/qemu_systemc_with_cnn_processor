#define SC_INCLUDE_DYNAMIC_PROCESSES

#include <inttypes.h>
#include "tlm_utils/simple_target_socket.h"

using namespace sc_core;
using namespace std;

#include "regs.h"
#include <sys/types.h>

regs::regs(sc_module_name name)
	: sc_module(name), socket("socket")
{
	socket.register_b_transport(this, &regs::b_transport);
	memset(&mmr, 0, sizeof mmr);
}

void regs::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
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
	
	cout << "Address = " << addr << endl;
        addr >>= 2;
        addr &= 7;
        if (trans.get_command() == tlm::TLM_READ_COMMAND) {
                memcpy(data, &mmr.u32[addr], len);
        } else if (cmd == tlm::TLM_WRITE_COMMAND) {
                memcpy(&mmr.u32[addr], data, len);
		switch (addr) {
			case 0:
				enable = mmr.enable;
				break;
			case 1:
				reset = mmr.reset;
				break;
			case 2: 
				cout << "test bench has  been enable" << endl;
				break;
			default:
				cout << "reached default case " << endl;
				break;
		}
        }
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
}
