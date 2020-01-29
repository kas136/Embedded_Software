#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ku_ipc_lib.c"

int main(void)
{
	int msqid1 = ku_msgget(1, 5);
	printf("msqid1 : %d\n", msqid1);
	int msqid2 = ku_msgget(2, 5);
	printf("msqid2 : %d\n", msqid2);
	int msqid3 = ku_msgget(3, 6);
	printf("msqid3 : %d\n", msqid3);
	int msqid4 = ku_msgget(1, 5);
	printf("msqid4 : %d\n", msqid4);
	int msqid5 = ku_msgget(2, 6);
	printf("msqid5 : %d\n", msqid5);
	int msqid6 = ku_msgget(3, 5);
	printf("msqid6 : %d\n", msqid6);

	struct MSGBUF {
		long type;
		char text[70];
	};

/*	struct MSGBUF *msgbuf = malloc(sizeof(struct MSGBUF));
	memset(msgbuf->text, '\0', sizeof(msgbuf->text));
	msgbuf->type = 1;
	msgbuf->text[0] = 'a';

	ku_msgsnd(msqid1, msgbuf, 1, 1);

	msgbuf->type = 1;
	msgbuf->text[0] = 'b';
	msgbuf->text[1] = 'c';

	ku_msgsnd(msqid1, msgbuf, 2, 1);
	ku_msgsnd(msqid1, msgbuf, 2, 1);
	ku_msgsnd(msqid1, msgbuf, 2, 1);
	ku_msgsnd(msqid1, msgbuf, 2, 0);
	ku_msgsnd(msqid1, msgbuf, 2, 1);
*/
	struct MSGBUF *msg1 = malloc(sizeof(struct MSGBUF));
	struct MSGBUF *msg2 = malloc(sizeof(struct MSGBUF));
	struct MSGBUF *msg3 = malloc(sizeof(struct MSGBUF));
	struct MSGBUF *msg4 = malloc(sizeof(struct MSGBUF));

	int a = ku_msgrcv(msqid1, msg1, 2, 1, 1);
	int b = ku_msgrcv(msqid1, msg2, 2, 1, 0);
	int c = ku_msgrcv(msqid1, msg3, 2, 1, 0);
	int d = ku_msgrcv(msqid1, msg4, 2, 1, 3);
	printf("a : %d\n", a);
	printf("b : %d\n", b);
	printf("c : %d\n", c);
	printf("d : %d\n", d);

	printf("type : %ld, msg1 : %c\n", msg1->type, msg1->text[0]);
	printf("type : %ld, msg2 : %c\n", msg2->type, msg2->text[0]);
	printf("type : %ld, msg3 : %c\n", msg3->type, msg3->text[0]);
	printf("type : %ld, msg4 : %c\n", msg4->type, msg4->text[0]);

}
