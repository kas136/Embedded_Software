#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#define DEV_NAME "ku_ipc_dev"
#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4

#define KU_IPC_NUM 'z'
#define KUMSGGET _IOWR(KU_IPC_NUM, IOCTL_NUM1, unsigned long *)
#define KUMSGCLOSE _IOWR(KU_IPC_NUM, IOCTL_NUM2, unsigned long *)
#define KUMSGSND _IOWR(KU_IPC_NUM, IOCTL_NUM3, unsigned long *)
#define KUMSGRCV _IOWR(KU_IPC_NUM, IOCTL_NUM4, unsigned long *)
#define SZ 30

int ku_msgget(int key, int msgflg)
{
	int dev;
	struct Arg {
		int key;
		int msgflg;
	};

	int temp;

	struct Arg arg;
	arg.key = key;
	arg.msgflg = msgflg;

	dev = open("/dev/ku_ipc_dev", O_RDWR);

	temp = ioctl(dev, KUMSGGET, &arg);

	close(dev);

	return temp;
}

int ku_msgclose(int msqid)
{
	int dev;
	int id = msqid;
	int temp;

	dev = open("/dev/ku_ipc_dev", O_RDWR);

	temp = ioctl(dev, KUMSGCLOSE, &id);

	close(dev);

	return temp;
}

int ku_msgsnd(int msqid, void *msgp, int msgsz, int msgflg)
{
	int dev;
	struct Arg {
		int id;
		void *p;
		int sz;
		int flg;
	};

	int temp;

	struct Arg arg;
	arg.id = msqid;
	arg.p = msgp;
	arg.sz = msgsz;
	arg.flg = msgflg;

	dev = open("/dev/ku_ipc_dev", O_RDWR);

	while(1)
	{
		temp = ioctl(dev, KUMSGSND, &arg);

		if(temp != 1)
		{
			break;
		}
	}

	close(dev);

	return temp;
}

int ku_msgrcv(int msqid, void *msgp, int msgsz, long msgtyp, int msgflg)
{
	int dev;
	struct Arg {
		int id;
		void *p;
		int sz;
		long typ;
		int flg;
	};

	int temp;

	struct Arg arg;
	arg.id = msqid;
	arg.p = msgp;
	arg.sz = msgsz;
	arg.typ = msgtyp;
	arg.flg = msgflg;

	dev = open("/dev/ku_ipc_dev", O_RDWR);

	while(1)
	{
		temp = ioctl(dev, KUMSGRCV, &arg);

		if(temp != 0)
		{
			break;
		}
	}

	close(dev);

	return temp;
}


