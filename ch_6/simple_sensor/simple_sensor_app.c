#include <stdio.h>
#include <sys/fcntl.h>

void main(void)
{
	int dev;
	char c;
	dev = open("/dev/simple_sensor_dev", O_RDWR);
	scanf("%c", &c);
	close(dev);
}
