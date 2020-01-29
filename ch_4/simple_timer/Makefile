KDIR := ~/linux-rpi
ARM := ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
PWD :=$(shell pwd)

obj-m := simple_timer.o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules

copy:
	scp simple_timer.ko pi@10.10.10.13:~

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean

