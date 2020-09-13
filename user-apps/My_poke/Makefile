CROSS_COMPILE=aarch64-linux-gnu-
CC=$(CROSS_COMPILE)gcc
GDB=gdb-multiarch
CFLAGS=-g
TARGET=my_poke
SOURCE=my_poke.c
QEMU_INSTANCE_HOSTNAME=qemu_zcu102
CONTAINER_HOSTNAME=peta@172.17.0.2
CONTAINER_STAGING_DIR=~/staging
CONTAINER_FORWARD_CMD='cd ${CONTAINER_STAGING_DIR} && ./forward ${TARGET}'

$(TARGET): $(SOURCE)
		$(CC) $(CFLAGS) -o $@ $^

all: $(TARGET)

upload: $(TARGET)
		@scp -q $^ $(CONTAINER_HOSTNAME):$(CONTAINER_STAGING_DIR)
		@ssh $(CONTAINER_HOSTNAME) $(CONTAINER_FORWARD_CMD)

clean:
		rm -rf *.o $(TARGET)

.PHONY: clean upload run kill
