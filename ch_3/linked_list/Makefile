KDIR := ~/linux-rpi
ARM := ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
PWD :=$(shell pwd)

obj-m := linked_list.o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules

copy:
	scp linked_list.ko pi@10.10.10.13:~

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean

