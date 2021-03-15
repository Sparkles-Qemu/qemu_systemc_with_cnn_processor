CROSS_COMPILE=aarch64-linux-gnu-
CC=$(CROSS_COMPILE)gcc
CFLAGS=-g
TARGET=udmabuf_irq_ram_test
SOURCE=udmabuf_irq_ram_test.c
QEMU_NAME=root
QEMU_IP=localhost

$(TARGET): $(SOURCE)
		$(CC) $(CFLAGS) -o $@ $^

all: $(TARGET)

upload: $(TARGET)
	scp -P 2222 $^  $(QEMU_NAME)@$(QEMU_IP):/home/root
ssh-reset:
	ssh-keygen -f "/home/peta/.ssh/known_hosts" -R [localhost]:2222
clean:
		rm -rf *.o $(TARGET)

.PHONY: clean upload run kill
