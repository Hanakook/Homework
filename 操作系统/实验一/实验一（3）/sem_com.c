
/* sem_com.c */

#include "sem_com.h"

int sem_create(int key,int num)
{
	int semid = semget(key, num, IPC_CREAT|0666);
	if (semid == -1)
		perror("sem_create error");
	return semid;
}

int sem_open(int key,int num)
{
	int semid = semget(key, num, IPC_CREAT|0666);
	if (semid == -1)
		perror("sem_open error");
	return semid;
}

int init_sem(int sem_id, int num ,int init_value)
{
	union semun sem_union;
	sem_union.val = init_value;
	if (semctl(sem_id, num, SETVAL, sem_union) == -1)
	{
		perror("Initialize semaphore");		
		return -1;
	}
	return 0;
}

int del_sem(int sem_id)
{
	union semun sem_union;
	if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
	{
		perror("Delete semaphore");
		return -1; 
	}
}

int sem_p(int sem_id,int num)//对信号灯集合中的第num（从0开始）个信号灯P操作
{
	struct sembuf sem_b;
	sem_b.sem_num = num; /*id*/
	sem_b.sem_op = -1; /* P operation*/
	sem_b.sem_flg = SEM_UNDO;
	
	if (semop(sem_id, &sem_b, 1) == -1) 
	{
		perror("P operation");
		return -1;
	}
	return 0;
}

int sem_v(int sem_id,int num)//v操作，参数同p操作
{
	struct sembuf sem_b;
	
	sem_b.sem_num = num; /* id */
	sem_b.sem_op = 1; /* V operation */	
	sem_b.sem_flg = SEM_UNDO; 

	if (semop(sem_id, &sem_b, 1) == -1)
	{
		perror("V operation");
		return -1;
	}
	return 0;
}
int sem_getval(int semid,int num)//获取第num个信号灯的值，参数同p操作
{
    int ret = semctl(semid, num, GETVAL, 0);
    if (ret == -1)
        perror("getval：");
    
    return 0;
}
