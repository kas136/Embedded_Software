#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#define DEV_NAME "ch2_mod1_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1

#define CH2_IOCTL_NUM 'z'
#define CH2_IOCTL1 _IOWR(CH2_IOCTL_NUM, IOCTL_NUM1, unsigned long *)

void main(void)
{
	int dev;
	unsigned long value = 0;

	dev = open("/dev/ch2_mod1_dev", O_RDWR);

	ioctl(dev, CH2_IOCTL1, &value);

	close(dev);
}
