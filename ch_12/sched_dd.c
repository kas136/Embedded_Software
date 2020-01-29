#define _GNU_SOURCE

#include <linux/kernel.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <linux/types.h>
#include <sched.h>
#include <linux/sched.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>

#define __NR_sched_setattr 314
#define __NR_sched_getattr 315
#define SCHED_DEADLINE 6
#define SCHED_BATCH 3
#define SCHED_FLAG_RESET_ON_FORK 0x01

typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed int s32;

struct sched_attr
{
	u32 size;

	u32 sched_policy;
	u64 sched_flags;

	s32 sched_nice;
	u32 sched_priority;
	u64 sched_runtime;
	u64 sched_deadline;
	u64 sched_period;
};

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags)
{
	return syscall(__NR_sched_setattr, pid, attr, flags);
}

int sched_getattr(pid_t pid, struct sched_attr *attr, unsigned int size, unsigned int flags)
{
	return syscall(__NR_sched_getattr, pid, attr, size, flags);
}

int main()
{
	int ret = 0;
	struct sched_attr param;

	ret = sched_getattr (0, &param, sizeof(struct sched_attr), 0);
	if(ret < 0)
	{
		printf("failed\n");
		return ret;
	}

	param.sched_policy = SCHED_DEADLINE;
	param.sched_runtime = 2000000;
	param.sched_deadline = 10000000;
	param.sched_period = 10000000;

	if(sched_setattr(0, &param, 0) == -1)
	{
		return -1;
	}

	printf("Changed scheduler - %d\n", sched_getscheduler(0));

	while(1)
	{
	}

	return 0;
}

