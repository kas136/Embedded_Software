#define KUIPC_MAXMSG 10
#define KUIPC_MAXVOL 25
#define KU_IPC_CREAT 5
#define KU_IPC_EXCL 6
#define KU_IPC_NOWAIT 1
#define KU_IPC_NOERROR 2

struct MSG {
	long type;
	char text[70];
};
