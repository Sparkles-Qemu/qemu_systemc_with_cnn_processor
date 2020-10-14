class regs
: public sc_core::sc_module
{
public:

  tlm_utils::simple_target_socket<regs> socket;
        
	
	//singal to enable and reset processor 
	sc_signal<bool> reset;
	sc_signal<bool> enable;


        regs(sc_core::sc_module_name name);
        virtual void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);

        union {
                struct {
                        uint32_t enable;
                        uint32_t reset;
                        uint32_t enable_tb;
                };
                uint32_t u32[4];
        }mmr;
};
