# THIS README CONTAINS BASIC INFO TO GET YOU STARTED DEVELOPING WITH SYSTEMC COSIMULATION

## MODELING CONCEPTS

Transaction level modeling in SystemC involves communication between SystemC processes using function calls.

## TARGET/INITIATOR SOCKETS

In TLM-2.0, an initiator is a module that initiates new transactions, and a target is a module that responds to transactions initiated by other modules. A transaction is a data structure (a C++ object) passed between initiators and targets using function calls. The same module can act both as an initiator and as a target, and this would typically be the case for a model of an arbiter, a router, or a bus.

In order to pass transactions between initiators and targets, TLM-2.0 uses sockets. An initiator sends transactions out through an initiator socket, and a target receives incoming transactions through a target socket. A module that merely forwards transactions without modifying their content is known as an interconnect component. An interconnect component would have both a target socket and an initiator socket.

Within the initiator and memory modules, the initiator and target sockets have to be declared and constructed explicitly, as follows:

class Initiator: sc_module
{
  tlm_utils::simple_initiator_socket<Initiator> socket;

  SC_CTOR(Initiator) : socket("socket")
  {
    ...
};

class Memory: sc_module
{
  tlm_utils::simple_target_socket<Memory> socket;

  SC_CTOR(Memory) : socket("socket")
  {
    ...
};

We also have to connect up the module hierarchy:

SC_MODULE(Top)
{
  Initiator *initiator;
  Memory    *memory;

  SC_CTOR(Top)
  {
    initiator = new Initiator("initiator");
    memory    = new Memory   ("memory");

    initiator->socket.bind( memory->socket );
  }
};

Notice the binding of the target socket to the initiator socket through the bind() function. But what about the callback function?

The initiator will communicate with the target memory using the blocking transport interface, so the target needs to implement a single method named b_transport. When using the simple target socket, this is done by having the target register a callback method with the socket as follows:

socket.register_b_transport(this, &Memory::b_transport);

All the target now has to do is to provide an implementation for the b_transport method. 

For a complete example of the above snippets go to : https://www.edaplayground.com/x/56Q4

Visual representation for visual learners:
![Init/target socket](Init_target socket.png.png)


## VIRTUAL BUS USED BY THE COSIMULATION

The iconnect bus is a virtual bus in the SystemC side that is in charge of sending data to the virtual devices(Modules) connected to it. 

The virtual bus is capable of supporting master and slave devices. The slave devices can only be written/read to from the QEMU side i.e there is  no way for the module to talk back to the QEMU side. Master devices in the other hand, in theory,  should be able to talk to the QEMU side(User application). A perfect example of a master device is a DMA. The DMA is programmed to readand write data to a Uer application, so there has to be a bi-directional communication between SystemC and QEMU


## ADDING YOUR OWN MODULES

This is a straight foward process. All you need to do is connect the target socket from the SystemC module to the virtual bus with the function XXX. This function is used to the map the module to a virtual address that can be writen/read to/from the QEMU side.

In order to communicate with the systemc modules from the QEMU side, you need to mmap to /dev/mem to write/read to the virtual address. From the QEMU side the virtual address that the module was mapped to is treated as a physical address. From a user application, the user can directly memcpy to this address. 


## COMPONENTS THAT YOU PLACE A BLACKBOX

 - Driver that takes care of sending payloads to the SytemC side( This was my biggest time waster)
 - Mechanism to communicate with QEMU from SystemC
 - Add images to show blackbox components 
 - Ask aly for more sections




