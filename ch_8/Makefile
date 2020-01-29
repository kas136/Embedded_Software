KDIR := ~/linux-rpi
ARM := ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
PWD := $(shell pwd)

obj-m := simple_rcu.o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules
	arm-linux-gnueabi-gcc -o reader reader.c
	arm-linux-gnueabi-gcc -o reader_rcu reader_rcu.c

copy:
	scp mknod.sh simple_rcu.ko reader reader_rcu pi@10.10.10.13:~

clean:
	$(MAKE) -C $(KDIR) M=$(PWD_ $(ARM) clean
	rm reader
	rm reader_rcu
	
