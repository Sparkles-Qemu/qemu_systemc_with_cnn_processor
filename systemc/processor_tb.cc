#include "processor_tb.h"


processor_tb::processor_tb(sc_module_name name, uint32_t *_done)
		: sc_module(name), done(_done)

{

	*(done) = 1;

	SC_THREAD(test_bench);
	dont_initialize();
	sensitive << clk.pos();

}

void processor_tb::test_bench()

{

	while(true) {
		wait();
		if(*(done) ==  0){
			 float expected_output[64] = {15678, 15813, 15948, 16083, 16218, 16353, 16488, 16623, 17028, 17163, 17298, 17433, 17568, 17703, 17838, 17973, 18378, 18513, 18648, 18783, 18918, 19053, 19188, 19323, 19728, 19863, 19998, 20133, 20268, 20403, 20538, 20673, 21078, 21213, 21348, 21483, 21618, 21753, 21888, 22023, 22428, 22563, 22698, 22833, 22968, 23103, 23238, 23373, 23778, 23913, 24048, 24183, 24318, 24453, 24588, 24723, 25128, 25263, 25398, 25533, 25668, 25803, 25938, 26073};

	cout  << " Data inside Processor module" << endl;  

  for(int i = 0; i < BIG_RAM_SIZE; i++) {

	cout<< processor->ramSource[i] << endl;
}

  // descriptors for source ram
 // Descriptor desc_mm2s = {0, 0, TRANSFER, BIG_RAM_SIZE, 1};

  // descriptors for destination ram 1
/*  Descriptor desc_s2mm1_wait_before = {1, 0, WAIT, 1, 1};
  Descriptor desc_s2mm1_transfer = {2, 0, TRANSFER, SMALL_RAM_SIZE, 1}; 
  Descriptor desc_s2mm1_sus_after = {0, 0, SUSPENDED, 0, 1};*/

  // descriptors for destination ram 2
  Descriptor desc_s2mm2_wait_before = {1, 0, WAIT, 1 + SMALL_RAM_SIZE, 1};
  Descriptor desc_s2mm2_transfer = {2, 0, TRANSFER, SMALL_RAM_SIZE, 1}; 
  Descriptor desc_s2mm2_sus_after = {0, 0, SUSPENDED, 0, 1};

  // descriptors for destination ram 3
  Descriptor desc_s2mm3_wait_before = {1, 0, WAIT, 1 + 2 * SMALL_RAM_SIZE, 1};
  Descriptor desc_s2mm3_transfer = {2, 0, TRANSFER, SMALL_RAM_SIZE, 1}; 
  Descriptor desc_s2mm3_sus_after = {0, 0, SUSPENDED, 0, 1};

  //branch0 descriptors
  Descriptor desc_branch0_group0_timed_wait = {1, 0, WAIT, BIG_RAM_SIZE, 1}; // 2
  Descriptor desc_branch0_group0_transfer = {2, 0, TRANSFER, IMAGE_SIZE, 1};
  Descriptor desc_branch0_group0_suspend = {2, 0, SUSPENDED, 0, 1};

  Descriptor desc_branch0_group1_timed_wait = {1, 0, WAIT, BIG_RAM_SIZE + 3, 1}; // 2
  Descriptor desc_branch0_group1_transfer = {2, 10, TRANSFER, IMAGE_SIZE, 1};
  Descriptor desc_branch0_group1_suspend = {2, 0, SUSPENDED, 0, 1};

  Descriptor desc_branch0_group2_timed_wait = {1, 0, WAIT, BIG_RAM_SIZE + 6, 1}; // 4
  Descriptor desc_branch0_group2_transfer = {2, 20, TRANSFER, IMAGE_SIZE, 1};
  Descriptor desc_branch0_group2_suspend = {2, 0, SUSPENDED, 0, 1};

  //branch1 descriptors
  Descriptor desc_branch1_group0_timed_wait = {1, 0, WAIT, BIG_RAM_SIZE + 9, 1}; // 8
  Descriptor desc_branch1_group0_transfer = {2, 0, TRANSFER, IMAGE_SIZE, 1};
  Descriptor desc_branch1_group0_suspend = {2, 0, SUSPENDED, 0, 1};


  Descriptor desc_branch1_group1_timed_wait = {1, 0, WAIT, BIG_RAM_SIZE + 12, 1};
  Descriptor desc_branch1_group1_transfer = {2, 10, TRANSFER, IMAGE_SIZE, 1};
  Descriptor desc_branch1_group1_suspend = {2, 0, SUSPENDED, 0, 1};


  Descriptor desc_branch1_group2_timed_wait = {1, 0, WAIT, BIG_RAM_SIZE + 15, 1};
  Descriptor desc_branch1_group2_transfer = {2, 20, TRANSFER, IMAGE_SIZE, 1};
  Descriptor desc_branch1_group2_suspend = {2, 0, SUSPENDED, 0, 1};

  //branch2 descriptors
  Descriptor desc_branch2_group0_timed_wait = {1, 0, WAIT, BIG_RAM_SIZE + 18, 1};
  Descriptor desc_branch2_group0_transfer = {2, 0, TRANSFER, IMAGE_SIZE, 1};
  Descriptor desc_branch2_group0_suspend = {2, 0, SUSPENDED, 0, 1};


  Descriptor desc_branch2_group1_timed_wait = {1, 0, WAIT, BIG_RAM_SIZE + 21, 1};
  Descriptor desc_branch2_group1_transfer = {2, 10, TRANSFER, IMAGE_SIZE, 1};
  Descriptor desc_branch2_group1_suspend = {2, 0, SUSPENDED, 0, 1};


  Descriptor desc_branch2_group2_timed_wait = {1, 0, WAIT, BIG_RAM_SIZE + 24, 1};
  Descriptor desc_branch2_group2_transfer = {2, 20, TRANSFER, IMAGE_SIZE, 1};
  Descriptor desc_branch2_group2_suspend = {2, 0, SUSPENDED, 0, 1};

  // File to trace down signals  
  // Start simulation 
  mmr->enable = 0;
  //reset = 1;
  
  //sc_start(0, SC_NS);
  //wait();
  //wait();

  //reset = 0;

 // sc_start(1, SC_NS);
  //wait();
  wait();

    // load descriptors
  //processor->left.dma_mm2s.loadProgram({desc_mm2s});
 /* processor->left.dma_s2mm1.loadProgram({desc_s2mm1_wait_before, desc_s2mm1_transfer, desc_s2mm1_sus_after});*/
  processor->left.dma_s2mm2.loadProgram({desc_s2mm2_wait_before, desc_s2mm2_transfer, desc_s2mm2_sus_after});
  processor->left.dma_s2mm3.loadProgram({desc_s2mm3_wait_before, desc_s2mm3_transfer, desc_s2mm3_sus_after});
  
  processor->right.branch0.group0.dma_mm2s.loadProgram({desc_branch0_group0_timed_wait, desc_branch0_group0_transfer, desc_branch0_group0_suspend});
  processor->right.branch0.group1.dma_mm2s.loadProgram({desc_branch0_group1_timed_wait, desc_branch0_group1_transfer, desc_branch0_group1_suspend});
  processor->right.branch0.group2.dma_mm2s.loadProgram({desc_branch0_group2_timed_wait, desc_branch0_group2_transfer, desc_branch0_group2_suspend});

  processor->right.branch1.group0.dma_mm2s.loadProgram({desc_branch1_group0_timed_wait, desc_branch1_group0_transfer, desc_branch1_group0_suspend});
  processor->right.branch1.group1.dma_mm2s.loadProgram({desc_branch1_group1_timed_wait, desc_branch1_group1_transfer, desc_branch1_group1_suspend});
  processor->right.branch1.group2.dma_mm2s.loadProgram({desc_branch1_group2_timed_wait, desc_branch1_group2_transfer, desc_branch1_group2_suspend});

  processor->right.branch2.group0.dma_mm2s.loadProgram({desc_branch2_group0_timed_wait, desc_branch2_group0_transfer, desc_branch2_group0_suspend});
  processor->right.branch2.group1.dma_mm2s.loadProgram({desc_branch2_group1_timed_wait, desc_branch2_group1_transfer, desc_branch2_group1_suspend});
  processor->right.branch2.group2.dma_mm2s.loadProgram({desc_branch2_group2_timed_wait, desc_branch2_group2_transfer, desc_branch2_group2_suspend});

  processor->right.branch0.group0.loadWeights({1,2,3});
  processor->right.branch0.group1.loadWeights({4,5,6});
  processor->right.branch0.group2.loadWeights({7,8,9});

  processor->right.branch1.group0.loadWeights({1,2,3});
  processor->right.branch1.group1.loadWeights({4,5,6});
  processor->right.branch1.group2.loadWeights({7,8,9});

  processor->right.branch2.group0.loadWeights({1,2,3});
  processor->right.branch2.group1.loadWeights({4,5,6});
  processor->right.branch2.group2.loadWeights({7,8,9});

  std::cout << "@" << sc_time_stamp() << " Load Pulse " << std::endl;

  //sc_start(0.5, SC_NS); 
  //wait();
  wait();
  //sc_start(0.5, SC_NS);

  mmr->enable = 1;
  
  std::cout << "@" << sc_time_stamp() << " Transfer Start " << std::endl;
  
  for (int i = 0; i < BIG_RAM_SIZE + 1; i++)
  {
    //sc_start(0.5, SC_NS);
    //sc_start(0.5, SC_NS);
     wait();
   //  wait();
  }

  std::cout << "@" << sc_time_stamp() << " Transfer Complete " << std::endl;


  volatile bool startValidation = false;
  volatile int expected_output_index = 0;
  volatile int ram_source_offset = 305; //some padding between input and output data
  volatile int validCounter = 8;
  volatile int invalidCounter = 1;
  int counter = 0;
  for(int k = 0; expected_output_index < 64 && k < 10000; ++k) {
    
    //sc_start(0.5, SC_NS);
    //sc_start(0.5, SC_NS);
    wait();
  //  wait();

    if(!startValidation && expected_output[0] == processor->streamOut)
    {
      startValidation = true;
      cout << "Validation started at k = " << k << endl;
    }

    if(startValidation)
    {
      if(validCounter > 0)
      {
        std::cout << "@" << sc_time_stamp() << " processor.streamOut: " << processor->streamOut \
        << " expected_output: " << expected_output[expected_output_index];

        if(expected_output[expected_output_index] == processor->streamOut)
        {
          std::cout << " ....assertion succuss!!" << std::endl;
          processor->ramSource[ram_source_offset + expected_output_index] = processor->streamOut;
          expected_output_index++;
        }
        else
        {
          std::cout << " ....assertion failure, terminating....!!" << std::endl;
          break;
        }
        validCounter--;
      }
      else if(invalidCounter > 0)
      {
        std::cout << "@" << sc_time_stamp() << " processor.streamOut: " << processor->streamOut \
        << " ....ignoring invalid output " << std::endl;
        invalidCounter--;
      }
      else
      {
        std::cout << "@" << sc_time_stamp() << " processor.streamOut: " << processor->streamOut \
        << " ....ignoring invalid output " << std::endl;
        validCounter = 8;
        invalidCounter = 1;
      }
    }
    
  }

  if(expected_output_index == 64)
  {
    std::cout << "TEST BENCH SUCCESS " << std::endl << std::endl;


    std::cout << "       aOOOOOOOOOOa" << std::endl;
    std::cout << "     aOOOOOOOOOOOOOOa" << std::endl;
    std::cout << "   aOO    OOOOOO    OOa" << std::endl;
    std::cout << "  aOOOOOOOOOOOOOOOOOOOa" << std::endl;
    std::cout << " aOOOOO   OOOOOO   OOOOOa" << std::endl;
    std::cout << "aOOOOO     OOOO     OOOOOa" << std::endl;
    std::cout << "aOOOOOOOOOOOOOOOOOOOOOOOOa" << std::endl;
    std::cout << "aOOOOOOOOOOOOOOOOOOOOOOOOa" << std::endl;
    std::cout << "aOOOOO   OOOOOOOO   OOOOOa" << std::endl;
    std::cout << " aOOOOO    OOOO    OOOOOa" << std::endl;
    std::cout << "  aOOOOO          OOOOOa" << std::endl;
    std::cout << "   aOOOOOOOOOOOOOOOOOOa" << std::endl;
    std::cout << "     aOOOOOOOOOOOOOOa" << std::endl;
    std::cout << "       aOOOOOOOOOOa" << std::endl;
	wait();
	wait();

  }
  else
  {
    std::cout << "TEST BENCH FAILURE " << std::endl  << std::endl;

    std::cout << "       aOOOOOOOOOOa" << std::endl;
    std::cout << "     aOOOOOOOOOOOOOOa" << std::endl;
    std::cout << "   aOOOOOOOOOOOOOOOOOOa" << std::endl;
    std::cout << "  aOOOOOOOOOOOOOOOOOOOOa" << std::endl;
    std::cout << " aOOOOO   OOOOOO   OOOOOa" << std::endl;
    std::cout << "aOOOOO     OOOO     OOOOOa" << std::endl;
    std::cout << "aOOOOOOO OOOOOOOOOOOOOOOOa" << std::endl;
    std::cout << "aOOOOOOO OOOOOOOOOOOOOOOOa" << std::endl;
    std::cout << "aOOOOOOOOOOOOOOOOOOOOOOOOa" << std::endl;
    std::cout << " aOOOOOO          OOOOOOa" << std::endl;
    std::cout << "  aOOOO  OOOOOOOO  OOOOa" << std::endl;
    std::cout << "   aOO OOOOOOOOOOOO OOa" << std::endl;
    std::cout << "     aOOOOOOOOOOOOOOa" << std::endl;
    std::cout << "       aOOOOOOOOOOa" << std::endl;
	wait();
	wait();
  }

	wait();
	*(done) = 1;

		}

	}

	



}
