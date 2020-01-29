KBUILD_EXTRA_SYMBOLS := /home/minhwan/ES/ch_2/mod1/Module.symvers
KDIR := ~/linux-rpi
ARM := ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
PWD := $(shell pwd)

obj-m := ch2_mod2.o

default:
		$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules

clean:
		$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean

