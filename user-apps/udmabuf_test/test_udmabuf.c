#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

//my includes

#define SYSTEMC_DEVICE_ADDR (0xA0010100)
#define DATA_SIZE	    (100)

int main(int argc, char *argv[])
{
	int fd, fd_dmabuf;
	char *base_ptr, *buf, ctr_flag = 1;
	unsigned val;
	unsigned addr, page_addr, page_offset;
	unsigned page_size=sysconf(_SC_PAGESIZE);
	
    unsigned char  attr[1024];
    unsigned int   buf_size, data_size = 100;

    //extract size
    if ((fd  = open("/sys/class/u-dma-buf/udmabuf0/size", O_RDONLY)) != -1) {
        read(fd, attr, 1024);
        sscanf(attr, "%d", &buf_size);
        close(fd);
    }

    //extract physical address
    unsigned int phys_addr, destination;
    if ((fd  = open("/sys/class/u-dma-buf/udmabuf0/phys_addr", O_RDONLY)) != -1) {
        read(fd, attr, 1024);
        sscanf(attr, "%x", &phys_addr);
        close(fd);
    }

    destination = phys_addr + 100;

    printf("Physical address of src = %d\n", phys_addr);
    printf("Physical address of dst = %d\n", destination);

    printf("size of long = %d\n", sizeof(unsigned int));

	//open virtual file to write to absolute address
	fd=open("/dev/mem",O_RDWR);
	if(fd<1) {
		perror(argv[0]);
		exit(-1);
	}

	page_addr=(SYSTEMC_DEVICE_ADDR & ~(page_size-1));
	page_offset=SYSTEMC_DEVICE_ADDR - page_addr;
	base_ptr = (char *)mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(SYSTEMC_DEVICE_ADDR & ~(page_size-1)));

    if ((fd_dmabuf = open("/dev/udmabuf0", O_RDWR))  == -1) {

        printf("Could not open /de/udmabuf0");
        close(fd);
    }
    
    buf = (char *)mmap(NULL, buf_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_dmabuf, 0);

    //fill buffer with some dummy data 
    printf("Filling up the buffer with dummy data\n");
    for(int i = 0; i < 100; i++){
        *(buf + i) = i + 1;
        printf("%d\n", *(buf + i));
    }


    //setting up demo_dma. use memcpy to write to device
	memcpy(base_ptr, &destination, sizeof(destination));	
    memcpy(base_ptr + 4, &phys_addr, sizeof(phys_addr));
    memcpy(base_ptr + 8, &data_size, sizeof(data_size));
    memcpy(base_ptr + 12, &ctr_flag, sizeof(ctr_flag));

       
	//printf("src adress = %x\n ", src);
	//printf("dst address = %x\n ", dst);
    sleep(2);

    for(int i = 100; i < data_size * 2; i++) {
        printf("%d\n", *(buf + i));
    }

    close(fd);
    close(fd_dmabuf);

	return 0; 
}
