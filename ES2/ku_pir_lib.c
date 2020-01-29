#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include "ku_pir.h"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4
#define IOCTL_NUM5 IOCTL_START_NUM+5

#define KU_PIR_NUM 'z'
#define KUPIROPEN _IOWR(KU_PIR_NUM, IOCTL_NUM1, unsigned long *)
#define KUPIRCLOSE _IOWR(KU_PIR_NUM, IOCTL_NUM2, unsigned long *)
#define KUPIRREAD _IOWR(KU_PIR_NUM, IOCTL_NUM3, unsigned long *)
#define KUPIRFLUSH _IOWR(KU_PIR_NUM, IOCTL_NUM4, unsigned long *)
#define KUPIRINSERTDATA _IOWR(KU_PIR_NUM, IOCTL_NUM5, unsigned long *)

int dev;

int ku_pir_open()
{
	int fd;
	int temp;

	dev = open("/dev/ku_pir_dev", O_RDWR);
	fd = dev;

	temp = ioctl(dev, KUPIROPEN, fd);

	return temp;
}

int ku_pir_close(int FD)
{
	int fd;
	int temp;

	fd = FD;

	temp = ioctl(dev, KUPIRCLOSE, fd);

	close(dev);

	return temp;
}

void ku_pir_read(int FD, struct ku_pir_data *data)
{
	struct Arg {
		int fd;
		struct ku_pir_data *rdata;
	};

	struct Arg arg;
	arg.fd = FD;
	arg.rdata = data;

	ioctl(dev, KUPIRREAD, &arg);

}

void ku_pir_flush(int FD)
{
	int fd;
	fd = FD;

	ioctl(dev, KUPIRFLUSH, fd);
}

int ku_pir_insertData(long unsigned int ts, char rf_flag)
{
	int temp;

	struct ku_pir_data arg;
	arg.timestamp = ts;
	arg.rf_flag = rf_flag;

	temp = ioctl(dev, KUPIRINSERTDATA, &arg);

	return temp;
}
