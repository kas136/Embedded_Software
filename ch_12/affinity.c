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
	cpu_set_t mask;

	int a = 0;

	CPU_ZERO(&mask);
	CPU_SET(cpu, &mask);
	pid = getpid();
	if(sched_setaffinity(pid, sizeof(mask), &mask)) printf("failed\n");
	else
	{
		while(1)
		{
			a++;
		}
	}
	return 0;
}
