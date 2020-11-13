CROSS_COMPILE=aarch64-linux-gnu-
CC=$(CROSS_COMPILE)gcc
CFLAGS=-g
TARGET=test_bench
SOURCE=test_bench.c
QEMU_NAME=root
QEMU_IP=localhost

$(TARGET): $(SOURCE)
		$(CC) $(CFLAGS) -o $@ $^

all: $(TARGET)

upload: clean $(TARGET) ssh-reset
	scp -P 2222 $^  $(QEMU_NAME)@$(QEMU_IP):/home/root

ssh-reset:
	ssh-keygen -f "/home/peta/.ssh/known_hosts" -R [localhost]:2222

debug: 
	ssh -p 2222 $(QEMU_NAME)@$(QEMU_IP) 'gdbserver localhost:2223 /home/root/$(TARGET)' 

clean:
		rm -rf *.o $(TARGET)

.PHONY: clean upload run kill
