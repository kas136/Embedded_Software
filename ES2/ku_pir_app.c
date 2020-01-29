#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ku_pir_lib.c"

int main(void)
{
	int a;
	int fd1 = ku_pir_open();
	int fd2 = ku_pir_open();

	struct ku_pir_data *data1 = malloc(sizeof(struct ku_pir_data));
	struct ku_pir_data *data2 = malloc(sizeof(struct ku_pir_data));

	ku_pir_insertData(10, '1');
	ku_pir_insertData(7, '1');
	ku_pir_insertData(10, '0');
	ku_pir_insertData(10, '1');
	ku_pir_insertData(10, '1');
	ku_pir_insertData(10, '1');
	ku_pir_insertData(10, '1');
	ku_pir_insertData(10, '1');
	ku_pir_insertData(10, '1');
	ku_pir_insertData(10, '1');
	ku_pir_insertData(10, '1');


	ku_pir_read(fd1, data1);
	printf("first fd : %d, ts : %d, flag : %c \n", fd1, data1->timestamp, data1->rf_flag);
	ku_pir_read(fd2, data2);
	printf("first fd : %d, ts : %d, flag : %c \n", fd2, data2->timestamp, data2->rf_flag);

	ku_pir_flush(fd1);
	ku_pir_read(fd1, data1);
	printf("first fd : %d, ts : %d, flag : %c \n", fd1, data1->timestamp, data1->rf_flag);

	ku_pir_close(fd2);
	
}
