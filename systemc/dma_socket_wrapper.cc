
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
#define USER_SPACE_DESCRIPTOR_SIZE 20 // in bytes
using namespace sc_core;
using namespace std;


#include "dma_socket_wrapper.h"

dma_socket_wrapper::dma_socket_wrapper(sc_module_name name, int size_)
	: sc_module(name), socket("socket")


{
	socket.register_b_transport(this, &dma_socket_wrapper::b_transport);
	socket.register_get_direct_mem_ptr(this, &dma_socket_wrapper::get_direct_mem_ptr);
	socket.register_transport_dbg(this, &dma_socket_wrapper::transport_dbg);

	size = size_;
	data_buf  = new uint8_t[size];
	memset(&data_buf[0], 0, size);


}	


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
		SC_REPORT_FATAL("Memory", "Unsupported access\n");
		return;
	}
	if (byt != 0) {
		trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		SC_REPORT_FATAL("Memory", "Unsupported access\n");
		return;
	}


	cout << "Address = " << addr << endl;
  cout << "Length = " << len << endl;
  cout << "Size of descriptor = " << sizeof(Descriptor) << endl;	

	if (trans.get_command() == tlm::TLM_READ_COMMAND)
		memcpy(ptr, &data_buf[addr], len);
	else if (cmd == tlm::TLM_WRITE_COMMAND)
		memcpy(&data_buf[addr], ptr, len);


  if(addr + len == USER_SPACE_DESCRIPTOR_SIZE) {
      cout << " Recieved a full descriptor" << endl;

      temp_descriptor = reinterpret_cast<Descriptor *>(data_buf);
      if(dma_ptr != NULL){
          dma_ptr->load_descriptor((*temp_descriptor));
          dma_ptr->print_descriptors();

      }else{
          cout<< "dma_ptr is NULL" << endl;
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

	dmi_data.set_dmi_ptr( reinterpret_cast<unsigned char*>(&data_buf[0]));
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(size - 1);
	/* Latencies are per byte.  Our latency is expressed per access,
	   which are in 32bits so dividie by 4. Is there a better way?.  */
	dmi_data.set_read_latency(LATENCY / 4);
	dmi_data.set_write_latency(LATENCY / 4);
	return true;
}

unsigned int dma_socket_wrapper::transport_dbg(tlm::tlm_generic_payload& trans)
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64    addr = trans.get_address();
	unsigned char*   ptr = trans.get_data_ptr();
	unsigned int     len = trans.get_data_length();
	unsigned int num_bytes = (len < (size - addr)) ? len : (size - addr);

	if (cmd == tlm::TLM_READ_COMMAND)
		memcpy(ptr, &data_buf[addr], num_bytes);
	else if ( cmd == tlm::TLM_WRITE_COMMAND )
		memcpy(&data_buf[addr], ptr, num_bytes);

	return num_bytes;
}
