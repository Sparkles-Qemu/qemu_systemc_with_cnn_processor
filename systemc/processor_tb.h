#include "Processor.h"


class processor_tb 
: public sc_core::sc_module	
{

public:
	//clock input
	sc_in<bool> clk;
	
	//internal singals
	sc_signal<bool> reset;
	sc_signal<bool> enable;	
	float *ramSource;
	uint32_t *done;
	
	Processor *processor;
	
	//constructor 
	processor_tb(sc_core::sc_module_name name, float *_ramSource, uint32_t *_done);
	void test_bench();
	
	SC_HAS_PROCESS(processor_tb);	

};
