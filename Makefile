obj-m += nyx.o

KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C &(KERNEL_DIR) m=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean

install: all
	sudo insmod nyx.ko

uninstall:
	sudo rmmod nyx

.PHONY: all clean install uninstall