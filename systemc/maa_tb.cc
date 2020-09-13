#include "maa.cc"



SC_MODULE (maa_tb) {
	sc_in<bool> clk; //clock signal is generated at top module so it needs to be an input 

	sc_signal <bool> enable;
    	sc_signal <float > pixel;
    	sc_signal <bool> reset;
    	sc_signal <float > partialsum;
    	sc_signal <float > out;
    	float out_test;
    	float weight = 3; //this can be an arbitrary number
    	int error = 0;
	
	int done = 1;	//controls the execution of the cthread function
	
	//pointer to module	
	mult_accumulate *maa;

	void test_bench(){

	while(true) {
		wait();
		if(done == 0) {
			// start test bench 
    			enable = 1;

    			for(int i = 0; i < 252; ++i){
        
        		partialsum = i;
        		pixel = 1;
        		//sc_start(1, SC_NS);
			wait();
		//	wait();
        		out_test = pixel.read()*weight + partialsum.read();
        		//sc_start(1, SC_NS)
			wait();
		//	wait();
        		if( out != out_test ) {
            			error++;
            			cout << "error ocurred inputs:"<<" pixel="<< pixel << " partialsum="<<partialsum << " out_test=" << out_test<< endl;
            			cout <<"expected value =" << out << endl;
        		}

    			}		

    			//test reset
    			reset = 0;
    			//sc_start(1, SC_NS);
			wait();
    			reset = 1;
    			//sc_start(1, SC_NS);
			wait();
			reset = 0;
    			if(out != 0) {
        			error++;
       				 cout<< "reset did not work"<<" out ="<< out<< endl;
    			}

    			//error checking
   			 if( error != 0 ) {
        			cout << "simulation falied"<< endl;
    			} else {
        			cout << "simulation passed" << endl;
    			}	
			wait();
			done = 1;

			}	
		
		}
	}

	SC_CTOR(maa_tb){
		maa = new mult_accumulate("maa");
		maa->clk	(clk);
		maa->reset	(reset);
		maa->enable	(enable);
		maa->partialSum	(partialsum);
		maa->pixelIn	(pixel);
		maa->output	(out);	
		maa->weight =   weight;

		SC_THREAD(test_bench);
		dont_initialize();
		sensitive << clk.pos();

	}	





};
