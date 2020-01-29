#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <sched.h>

int main(void)
{
	pid_t pid;
	int cpu = 0;
	int scheduler;
	cpu_set_t mask;
	struct sched_param param;
	int a = 0;

	CPU_ZERO(&mask);
	CPU_SET(cpu, &mask);
	pid = getpid();
	if(sched_setaffinity(pid, sizeof(mask), &mask)) printf("failed\n");

	else
	{
		param.sched_priority = sched_get_priority_min(SCHED_FIFO);
		if(sched_setscheduler(0, SCHED_FIFO, &param)==-1)
		{
			printf("failed2\n");
		}

		else
		{
			scheduler = sched_getscheduler(0);
			switch(scheduler)
			{
				case SCHED_OTHER:
					printf("default scheduling is being used\n");
					break;
				case SCHED_FIFO:
					printf("fifo scheduling is being used\n");
					break;
				case SCHED_RR:
					printf("round robin scheduling is being used\n");
					break;
			}
			for(int i = 0; i <500000000; i++)
			{
				a++;
			}

			pid = fork();
			printf("forked\n");
			if(pid == 0)
			{
				param.sched_priority = sched_get_priority_max(SCHED_FIFO);
				if(sched_setparam(0, &param)) printf("failed3\n");

				else
				{
					while(1)
					{
						a++;
					}
				}
			}

			else
			{
				sleep(2);
				while(1)
				{
					a++;
				}
			}
		}
	}
	return 0;
}

