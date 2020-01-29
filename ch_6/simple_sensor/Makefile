KDIR := ~/linux-rpi
ARM := ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
PWD :=$(shell pwd)

obj-m := simple_sensor.o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules

copy:
	scp simple_sensor.ko mknod.sh simple_sensor_app pi@10.10.10.13:~/


clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean

