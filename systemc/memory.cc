/*
 * Copyright (c) 2011 Edgar E. Iglesias.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include <inttypes.h>

#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"

using namespace sc_core;
using namespace std;

#include "memory.h"

memory::memory(sc_module_name name, sc_time latency, int size_)
	: sc_module(name), socket("socket"), LATENCY(latency) 


{
	socket.register_b_transport(this, &memory::b_transport);
	socket.register_get_direct_mem_ptr(this, &memory::get_direct_mem_ptr);
	socket.register_transport_dbg(this, &memory::transport_dbg);

	size = size_;
	mem = new float[size];
	memset(&mem[0], 0, size);
	//memset(&regs, 0, sizeof regs);

	// signals and processor
	//processor("processor", this->mem, this->clk, this->reset, this->enable);

	SC_THREAD(accelerate);
	dont_initialize();
	sensitive << processor_start;
}	

void memory::accelerate()
{
	while(true) {
		if(!(identifier == BIG_RAM_SIZE)){
			wait(processor_start);
		}
		printf("made it to accelerate\n");
		identifier = 0;
		cout<<"Data right before sending it to  Processor" <<  endl;

		//processor->compute(reset, enable);
		//maa->test_bench();
		processor->done = 0;
	}

}

void memory::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64    addr = trans.get_address();
	unsigned char*   ptr = trans.get_data_ptr();
	unsigned int     len = trans.get_data_length();
	unsigned char*   byt = trans.get_byte_enable_ptr();
	int  index;
	int user_data;

	if (addr > sc_dt::uint64(size)) {
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		SC_REPORT_FATAL("Memory", "Unsupported access\n");
		return;
	}
	if (byt != 0) {
		trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		SC_REPORT_FATAL("Memory", "Unsupported access\n");
		return;
	}


	cout << "Address = " << addr << endl;
	
	//since our mem array is of type float now, we have to devide addr by  4
	index = addr / 4;
	user_data = *(reinterpret_cast<int *>(ptr));

	cout << "data = " << user_data << endl;
	/*if(index >= 0 && index < 3) {   //this is a mmr write or read

		if (trans.get_command() == tlm::TLM_READ_COMMAND)
                	memcpy(ptr, &regs.u32[index], len);
        	else if (cmd == tlm::TLM_WRITE_COMMAND)
                	regs.u32[index] = user_data;
	}*/

	//Memory write or read, we need to "normalize" index again
	//	index -= 3;
	
	if (trans.get_command() == tlm::TLM_READ_COMMAND)
		memcpy(ptr, &mem[index], len);
	else if (cmd == tlm::TLM_WRITE_COMMAND)
		mem[index] = user_data;

	delay += LATENCY;
	//identifier++;
	
	//trigger sc_thread

	/*if(identifier == BIG_RAM_SIZE) {
		printf("Identifier = %d\n", identifier);
		processor_start.notify();
	}*/

	trans.set_dmi_allowed(true);
	trans.set_response_status(tlm::TLM_OK_RESPONSE);
}

bool memory::get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
				tlm::tlm_dmi& dmi_data)
{
	dmi_data.allow_read_write();

	dmi_data.set_dmi_ptr( reinterpret_cast<unsigned char*>(&mem[0]));
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(size - 1);
	/* Latencies are per byte.  Our latency is expressed per access,
	   which are in 32bits so dividie by 4. Is there a better way?.  */
	dmi_data.set_read_latency(LATENCY / 4);
	dmi_data.set_write_latency(LATENCY / 4);
	return true;
}

unsigned int memory::transport_dbg(tlm::tlm_generic_payload& trans)
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64    addr = trans.get_address();
	unsigned char*   ptr = trans.get_data_ptr();
	unsigned int     len = trans.get_data_length();
	unsigned int num_bytes = (len < (size - addr)) ? len : (size - addr);

	if (cmd == tlm::TLM_READ_COMMAND)
		memcpy(ptr, &mem[addr], num_bytes);
	else if ( cmd == tlm::TLM_WRITE_COMMAND )
		memcpy(&mem[addr], ptr, num_bytes);

	return num_bytes;
}
