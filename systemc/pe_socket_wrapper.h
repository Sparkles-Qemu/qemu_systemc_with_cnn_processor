#ifndef PE_GROUP_H
#define PE_GROUP_H

#include "Right.cc"

class pe_socket_wrapper
: public sc_core::sc_module
{
public:
	tlm_utils::simple_target_socket<pe_socket_wrapper> socket;
	PE_GROUP *pe_group_ptr = NULL;
  std::vector<float> weights;
  int message_count;

	pe_socket_wrapper(sc_core::sc_module_name name);
	virtual void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);
};

#endif
