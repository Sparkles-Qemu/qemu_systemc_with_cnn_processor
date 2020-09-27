#ifndef DMA_CPP // Note include guards, this is a quick and dirty way to include components
#define DMA_CPP

#include <systemc.h>
#include "map"
#include "vector"
#include <string>
#include <iostream>
#include "tlm_utils/simple_target_socket.h"

//-----------------------------------------------------
// Design Name : DMA
// File Name   : DMA.cpp
// Function    : Direct Memory Access
// Coder       : Jacob Londa and Owen Beringer
//-----------------------------------------------------

enum class DmaDirection
{
  MM2S,
  S2MM
};

enum class DmaState
{
  SUSPENDED, // do nothing indefinitely
  TRANSFER,  // transfer data
  TRANSFER_WITH_FORWARD,
  WAIT       // do nothing for certain number of cycles
};

struct Descriptor
{
  unsigned int next;     // index of next descriptor
  unsigned int start;    // start index in ram array
  DmaState state;        // state of dma
  unsigned int x_count;  // number of floats to transfer/wait
  unsigned int x_modify; // number of floats between each transfer/wait
};

// DMA module definition for MM2S and S2MM
struct DMA 
: public sc_module
{
public:
  // Control Signals
  sc_in<bool> clk, reset, enable;

  // Memory and Stream
  float *ram;
  sc_inout<float> stream;

  // Internal Data
  std::vector<Descriptor> descriptors;
  unsigned int execute_index;
  DmaDirection direction;
  unsigned int current_ram_index;
  unsigned int x_count_remaining;
	int descriptor_size;
	int descriptor_size_word_align;
	unsigned char *mem;

  const Descriptor default_descriptor = {0, 0, DmaState::SUSPENDED, 0, 0};

  //Target socket and callback function
  tlm_utils::simple_target_socket<DMA> socket;
	void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
	{
  tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64    addr = trans.get_address();
	unsigned char*   ptr = trans.get_data_ptr();
	unsigned int     len = trans.get_data_length();
	unsigned char*   byt = trans.get_byte_enable_ptr();

	if (addr > sc_dt::uint64(descriptor_size_word_align)) {
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		SC_REPORT_FATAL("Memory", "Unsupported access\n");
		return;
	}
	if (byt != 0) {
		trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		SC_REPORT_FATAL("Memory", "Unsupported access\n");
		return;
	}

  //
  cout << "Made it to the DMA target socket callback " << endl;

	if (trans.get_command() == tlm::TLM_READ_COMMAND)
		memcpy(ptr, &mem[addr], len);
	else if (cmd == tlm::TLM_WRITE_COMMAND)
		memcpy(&mem[addr], ptr, len);

	//delay += LATENCY;

	trans.set_dmi_allowed(true);
	trans.set_response_status(tlm::TLM_OK_RESPONSE);

	}

  // Prints descriptor list, useful for debugging
  void print_descriptors()
  {
    for (uint i = 0; i < descriptors.size(); i++)
    {
      std::cout << "Descriptor " << i << std::endl;
      std::cout << "next: " << descriptors[i].next << std::endl;
      std::cout << "start: " << descriptors[i].start << std::endl;
      std::cout << "state: " << (int)descriptors[i].state << std::endl;
      std::cout << "x_count: " << descriptors[i].x_count << std::endl;
      std::cout << "x_modify: " << descriptors[i].x_modify << std::endl
                << std::endl;
    }
  }

  void loadProgram(std::vector<Descriptor> newProgram)
  {
    descriptors.clear();
    for (unsigned int i = 0; i < newProgram.size(); i++)
    {
      descriptors.push_back(newProgram[i]);
    }
    execute_index = 0;
    current_ram_index = descriptors[execute_index].start;
    x_count_remaining = descriptors[execute_index].x_count;
  }

  // Called on rising edge of clk or high level reset
  void update()
  {
    if (reset.read())
    {
      // assume at least one descriptor is in dma at all times
      execute_index = 0;
      descriptors.clear();
      descriptors.push_back(default_descriptor);
      current_ram_index = descriptors[execute_index].start;
      x_count_remaining = descriptors[execute_index].x_count;
      descriptors[execute_index].state = DmaState::SUSPENDED; // slightly cheating here, but does what we want
      std::cout << "@ " << sc_time_stamp() << " " << this->name() << ": Module has been reset" << std::endl;
    }
    else if (enable.read() && (descriptors[execute_index].state != DmaState::SUSPENDED))
    {
      if (descriptors[execute_index].state == DmaState::TRANSFER)
      {
        if (direction == DmaDirection::MM2S) // Memory to Stream
        {
          float value = *(ram + current_ram_index);
          // std::cout << "@ " << sc_time_stamp() << " " << this->name() << " desc " << execute_index << ": Transfering [" << value << "] from RAM to stream" << std::endl;
          stream.write(value);
        }
        else // Stream to Memory
        {
          // std::cout << "@ " << sc_time_stamp() << " " << this->name() << " desc " << execute_index << ": Transfering [" << stream.read() << "] from stream to RAM" << std::endl;
          *(ram + current_ram_index) = stream.read();
        }

        // update ram index
        current_ram_index += descriptors[execute_index].x_modify;
      }
      else // Waiting state
      {
        if (direction == DmaDirection::MM2S) // clear stream only for MMM2S
          stream.write(0);
        // std::cout << "@ " << sc_time_stamp() << " " << this->name() << " desc " << execute_index << ": Waiting..." << std::endl;
      }

      x_count_remaining--;

      if (x_count_remaining == (unsigned int)0) // descriptor is finished, load next descriptor
      {
        execute_index = descriptors[execute_index].next;
        current_ram_index = descriptors[execute_index].start;
        x_count_remaining = descriptors[execute_index].x_count;
      }
    }
    else // Suspended state
    {
      if (direction == DmaDirection::MM2S) // clear stream only for MMM2S
        stream.write(0);
    }
  }

  // Constructor
  DMA(sc_module_name name, DmaDirection _direction, const sc_signal<bool> &_clk, const sc_signal<bool> &_reset, const sc_signal<bool> &_enable, float *_ram, sc_signal<float, SC_MANY_WRITERS> &_stream) : sc_module(name)
  {
    // std::cout << "DMA Module: " << name << " attempting to instantiate " << std::endl;

    SC_METHOD(update);
    sensitive << reset;
    sensitive << clk.pos();

    // connect signals
    this->direction = _direction;
    this->clk(_clk);
    this->reset(_reset);
    this->enable(_enable);
    this->ram = _ram;
    this->stream(_stream);

		this->descriptor_size = sizeof(Descriptor);
		this->descriptor_size_word_align = ((descriptor_size / 4) + 1) * 4;

		this->mem = new unsigned char[descriptor_size_word_align];
    std::cout << "DMA Module: " << name << " has been instantiated " << std::endl;
  }

 DMA(sc_module_name name, DmaDirection _direction, const sc_signal<bool> &_reset, const sc_signal<bool> &_enable, float *_ram, sc_signal<float, SC_MANY_WRITERS> &_stream) : sc_module(name)
  {
    // std::cout << "DMA Module: " << name << " attempting to instantiate " << std::endl;

    SC_METHOD(update);
    sensitive << reset;
    sensitive << clk.pos();

    // connect signals
    this->direction = _direction;
    this->reset(_reset);
    this->enable(_enable);
    this->ram = _ram;
    this->stream(_stream);

    std::cout << "DMA Module: " << name << " has been instantiated " << std::endl;
  }
  // Constructor
  DMA(sc_module_name name) : sc_module(name)
  {
    // std::cout << "DMA Module: " << name << " attempting to instantiate " << std::endl;

    SC_METHOD(update);
    sensitive << reset;
    sensitive << clk.pos();

    std::cout << "DMA Module: " << name << " has been instantiated with empty constructor" << std::endl;
  }

  SC_HAS_PROCESS(DMA);
};

// DMA module definition for MM2MM
class DMA_MM2MM : public sc_core::sc_module
{
  // stream interconnect
  sc_signal<float, SC_MANY_WRITERS> stream;

  // DMA devices
  DMA mm2s, s2mm;

  // Constructor
  DMA_MM2MM(sc_core::sc_module_name name, const sc_signal<bool> &_clk, const sc_signal<bool> &_reset, const sc_signal<bool> &_enable, float *_ram_source, float *_ram_destination) : sc_module(name),
                                                                                                                                                                                     mm2s("internal_mm2s", DmaDirection::MM2S, _clk, _reset, _enable, _ram_source, stream),
                                                                                                                                                                                     s2mm("internal_s2mm", DmaDirection::S2MM, _clk, _reset, _enable, _ram_destination, stream)
  {
    std::cout << "Module: " << name << " has been instantiated" << std::endl;
  }

  SC_HAS_PROCESS(DMA_MM2MM);
};


#endif // DMA_CPP
