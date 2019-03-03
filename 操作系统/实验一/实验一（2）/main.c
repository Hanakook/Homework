#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<unistd.h>
#include<string.h>
#include"sem_com.h"
#define buf_size 40
#define shm_key 9399
#define sem_key1 9393
#define sem_key2 9595
#define sem_key3 9494
#define n 10
int count=0;
int in=0;
int	out=0;
struct Buffer		//定义一个共享内存存储结构
{
	char data[buf_size];
};


void Productor(struct Buffer *buf)
{
	const char *str="product";
		char* b;
		count++;
	strcpy(buf->data,str);
	sprintf(b,"%d",count);
	strcat(buf->data,b);
	printf("Productor:%s\n",buf->data);
	in=(in+1)%n;
}

void Customer(struct Buffer *buf)
{
    printf("Customer:%s\n",buf->data);
    out=(out+1)%n;
}

int main()
{
	int empty,full,mutex,shmid[n];
	empty=sem_create(sem_key1,1);//创建信号量
	full=sem_create(sem_key2,1);
	mutex=sem_create(sem_key3,1);
	init_sem(empty,0,n);//初始化信号量
	init_sem(full,0,0);
	init_sem(mutex,0,1);
	for(int i=0;i<n;i++)//创建n个共享内存作为缓冲区
	{
		shmid[i]=shmget(ftok("zh", 254),sizeof(struct Buffer),0666|IPC_CREAT);
		if(shmid[i]==-1)
		{
			printf("%d\n",i);
			perror("shmget error");
			exit(-1);
		}
	}
	if(0==fork())
	{
	   	struct Buffer *buf[n];
		for(int i=0;i<n;i++)
		{
			printf("%d\n",i);
		void * tmp=shmat(shmid[i],NULL,0);//映射共享内存
	if(tmp==(void *)-1)
        {
			perror("shmat error");
			exit(-1);
		}
		buf[i]=(struct Buffer *)tmp;
		}
	   	for(int i=0;i<5;i++)
		{
			//sleep(2);
           printf("%d\n",i);
			sem_p(empty,0);
			sem_p(mutex,0);
			puts("---");
			Productor(buf[in]);
			sem_v(mutex,0);
			sem_v(full,0);
		}
	}
	else
	{
		 
	   	struct Buffer *buf[n];
		for(int i=0;i<n;i++)
		{
		void * tmp=shmat(shmid[i],NULL,0);//映射共享内存
		buf[i]=(struct Buffer *)tmp;
		}
	   	for(int i=0;i<5;i++)
     	{
			printf("%d\n",i);
			sem_p(full,0);
	//		puts("___");
			sem_p(mutex,0);
			Customer(buf[out]);
			sem_v(mutex,0);
			sem_v(empty,0);

		}
	}
}
	

