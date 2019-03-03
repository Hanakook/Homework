
/* sem_com.h */

#ifndef		SEM_COM_H
#define		SEM_COM_H

#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};
int sem_create(int,int);
int sem_open(int,int);
int init_sem(int, int,int);
int sem_getval(int ,int );
int del_sem(int);
int sem_p(int,int);
int sem_v(int,int); 

#endif /* SEM_COM_H */
