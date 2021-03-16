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

/*enum class DmaState
	{
	SUSPENDED, // do nothing indefinitely
	TRANSFER,  // transfer data
	TRANSFER_WITH_FORWARD,
	WAIT       // do nothing for certain number of cycles
	};*/

#define SUSPENDED             (0)
#define TRANSFER              (1)
#define TRANSFER_WITH_FORWARD (2)
#define WAIT                  (3)

struct Descriptor
{
	uint32_t next;     // index of next descriptor
	uint32_t start;    // start index in ram array
	uint32_t state;    // state of dma
	uint32_t x_count;  // number of floats to transfer/wait
	uint32_t x_modify; // number of floats between each transfer/wait
};

// DMA module definition for MM2S and S2MM
struct DMA : public sc_module
{

	// Control Signals
	sc_in<bool> clk, reset, enable;

	// Memory and Stream
	float *ram;
	sc_inout<float> stream;

	//transfer finish signal 
	sc_out<bool> trans_finished;
	// Internal Data
	std::vector<Descriptor> descriptors;
	unsigned int execute_index;
	DmaDirection direction;
	unsigned int current_ram_index;
	unsigned int x_count_remaining;
	unsigned int descriptor_count = 0;
	const Descriptor default_descriptor = {0, 0, SUSPENDED, 0, 0};

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

	void load_descriptor(Descriptor newDescriptor){ 

		if(descriptor_count == 0){
			descriptors.clear(); //clears default descriptor
		}
		descriptors.push_back(newDescriptor);
		descriptor_count++;

		//First descriptor
		if(descriptor_count == 1){
			execute_index = 0;
			current_ram_index = descriptors[execute_index].start;
			x_count_remaining = descriptors[execute_index].x_count;
		}
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
			descriptor_count = 0;
			descriptors[execute_index].state = SUSPENDED; // slightly cheating here, but does what we want
			std::cout << "@ " << sc_time_stamp() << " " << this->name() << ": Module has been reset" << std::endl;
		}
		else if (enable.read() && (descriptors[execute_index].state != SUSPENDED))
		{
			if (descriptors[execute_index].state == TRANSFER)
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
				trans_finished.write(1);
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

		std::cout << "DMA Module: " << name << " has been instantiated " << std::endl;
	}

	DMA(sc_module_name name, DmaDirection _direction, const sc_signal<bool> &_reset, const sc_signal<bool> &_enable, float *_ram) : sc_module(name) 
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
struct DMA_MM2MM : public sc_module
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
