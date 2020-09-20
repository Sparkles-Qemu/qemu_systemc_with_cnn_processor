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

upload: $(TARGET)
	scp -P 2222 $^  $(QEMU_NAME)@$(QEMU_IP):/home/root
clean:
		rm -rf *.o $(TARGET)

.PHONY: clean upload run kill
