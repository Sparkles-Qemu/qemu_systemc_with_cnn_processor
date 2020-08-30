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

Visual representation of taret/init sockets:

![Init/target socket](/Init_target%20socket.png)



## ADDING YOUR OWN MODULES

This is a straight foward process. All you need to do is connect the target socket from the SystemC module to the virtual bus with the function memmap(). This function is used to map the module to a virtual address that can be writen/read to/from the QEMU side.

Example from Xilinx co-simulation:

bus.memmap(0xa0000000ULL, 0x100 - 1,
				ADDRMODE_RELATIVE, -1, debug.socket);

In this example the virtual bus is named "bus" and the user maps a SystemC module named debug. The memmap function takes care of mapping the target socket from the debug device(debug.socket) to the virtual address 0xa0000000ULL. The second parameter, 0x100 - 1, is used to describe the size of each transaction that this module can handle. The third parameter is the mode of the device. Xilinx recommends always using RELATIVE mode. The fourth parameter tells the memmap function if you want to bind the target socket to a specific initiator socket within the virtual bus. In most cases, you will end up using -1 because you do not really care to which initiator socket the target socket is binded to; you only care that the target gets binded. 

Visual Representation of virtual bus:

![Virtual Bus](/Virtual%20Bus%20.png)

In order to communicate with the systemc modules from the QEMU side, you need to mmap to /dev/mem to write/read to the virtual address. From the QEMU side the virtual address that the module was mapped to is treated as a physical address. From a user application, the user can directly memcpy to this address. 

User level application example:

unsigned page_size=sysconf(_SC_PAGESIZE); //we are mapping a block that is a the size of a page

//open virtual file to write to absolute address

fd=open("/dev/mem",O_RDWR);
if(fd<1) {
  exit(-1);
}

base_ptr = mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd, SYSTEMC_DEVICE_ADDR);

if(base_ptr == NULL) {
	//error handling 
}

memcpy(base_ptr, [USER_DATA], sizeof USER_DATA); //USER_DATA IT'S A PLACE HOLDER

In this example, SYSTEMC_DEVICE_ADDR represents the addressed that the SystemC module was mapped to by the virtual bus. After the user has correctly obtained a pointer through mmap, the user can do direct read and writes to this address.

## COMPONENTS THAT YOU CAN BLACKBOX

The point of this section is to save you time when something goes wrong. There are many components that are hidden from the user in the co-simulation. This is a good thing as long as you do not get side track by them.

One of the major components that is abstracted from the user is the driver that takes care of seding the payload to the SystemC side. All you need to know is that it exist and it works. If for some reason you find yourself trying to figure out what is happening in the driver side, you are wasting your time. If there is an  error(unexpected behavior) between QEMU and SystemC, 99% of time is an user error and not a driver error.


Visual represention of blackbox:
![Blackbox](/Blackbox.png)
