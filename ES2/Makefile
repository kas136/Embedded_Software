KDIR := ~/linux-rpi
ARM := ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
PWD :=$(shell pwd)

obj-m := ku_pir.o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules
	arm-linux-gnueabi-gcc -o ku_pir_app ku_pir_app.c
	arm-linux-gnueabi-gcc -o ku_app ku_app.c
	
copy:
	scp ku_pir.ko ku_pir_app ku_app ku_pir_mknod.sh pi@10.10.10.13:~

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean

