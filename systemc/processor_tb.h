#include "Processor.h"
#include "regs.h"


class processor_tb 
: public sc_core::sc_module	
{

public:
	//clock input
  sc_in<bool> clk;
	
	//internal singals
  //sc_signal<bool> reset;
	//sc_signal<bool> enable;	
	float *ramSource;
	uint32_t *done;
  regs *mmr;
	
	Processor *processor;
	
	//constructor 
	processor_tb(sc_core::sc_module_name name, uint32_t *_done);
	void test_bench();
	
	SC_HAS_PROCESS(processor_tb);	

};
