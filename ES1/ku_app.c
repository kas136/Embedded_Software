#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ku_ipc_lib.c"

int main(void)
{
	int msqid1 = ku_msgget(1, 5);

	struct MSGBUF {
		long type;
		char text[70];
	};

	struct MSGBUF *msg1 = malloc(sizeof(struct MSGBUF));
	memset(msg1->text, '\0', sizeof(msg1->text));
	msg1->type = 1;
	msg1->text[0] = 'k';
	msg1->text[1] = 'p';
	msg1->text[2] = 'r';
	msg1->text[3] = 'w';
	msg1->text[4] = 'e';

	ku_msgsnd(msqid1, msg1, 2, 1);
	ku_msgsnd(msqid1, msg1, 5, 1);
	//ku_msgsnd(msqid1, msg1, 2, 1);
	//ku_msgsnd(msqid1, msg1, 2, 1);
	//ku_msgsnd(msqid1, msg1, 2, 0);

	printf("finish\n");
}
