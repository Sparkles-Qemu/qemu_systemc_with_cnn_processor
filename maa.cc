// All includes 
#include <systemc.h>

// Module defintion 
class mult_accumulate : public sc_module 
{
public:
    //inputs
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<bool> enable;
    sc_in<float> partialSum;
    sc_in<float> pixelIn;

    //outputs
    sc_out<float > output;

 
    float weight;
    float result;

    //function to update the output value
    void update() {

        if(reset.read() == 1) {
            output.write(0);
        } else if (enable.read() == 1) {
            result = pixelIn.read() * weight + partialSum.read();
            output.write(result);
        }
    }


    //function to monitor value changes on output value
    void monitor() {
        cout<<"@"<< sc_time_stamp() << "Output =" << output.read()<<endl;
    }
	
  // void test_bench();
    SC_HAS_PROCESS(mult_accumulate);

    //Constructor for module. This module is pos edge trigger 
    mult_accumulate(sc_module_name name) : sc_module(name), clk("clk"), reset("reset"), enable("enable"), partialSum("partialSum"), pixelIn("pixelIn"), output("output") {
      // std::cout << "MAC Module " << name << " attempting to instantiate " << std::endl;

       SC_METHOD(update);
            sensitive << clk.pos();
            sensitive << reset;
	//SC_METHOD(monitor);
	//	sensitive << clk.pos();
      std::cout << "MAC Module " << name << " instantiated " << std::endl;
    }
};
