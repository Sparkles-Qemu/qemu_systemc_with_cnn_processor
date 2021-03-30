CROSS_COMPILE=aarch64-linux-gnu-
CC=$(CROSS_COMPILE)gcc
CFLAGS=-g
OBJS :=test_bench.o io_access_lib.o user_lvl_driver.o
TARGET=test_bench
SOURCE=test_bench.c
QEMU_NAME=root
QEMU_IP=localhost


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $< 

upload: $(TARGET)
	scp -P 2222 $^  $(QEMU_NAME)@$(QEMU_IP):/home/root
ssh-reset:
	ssh-keygen -f "/home/peta/.ssh/known_hosts" -R [localhost]:2222
clean:
	rm -rf *.o $(TARGET)

.PHONY: clean upload run kill
