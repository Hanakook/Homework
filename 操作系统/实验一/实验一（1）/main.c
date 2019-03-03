#include<sys/types.h>
#include<stdio.h>
#include<sys/sem.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/stat.h>
#include<unistd.h>
#include <sys/wait.h>
#include"sem_com.h"
int main()
{
	int semid,i;
    semid=sem_open( ftok("zhj", 253), 5);//创建5个信号灯
	if ( semid == -1 ) exit(-1);
	for(i=0;i<5;i++)//给5个信号灯赋初值
	{
		if ( init_sem(semid,i,0) == -1 ) {
			exit(-1);
		}
	}
	if(0==fork())
	{
        sem_p(semid,0);
		printf("This is second child progress\n");	
	}
	else
	{
		sem_v(semid,0);
		sem_v(semid,1);
		sem_v(semid,2);
		printf("This is first progress\n");
		if(0 == fork())		{
			sem_p(semid,1);
			printf("This is third child progress\n");
			sem_v(semid,3);
		}		
		else{

			if(0== fork())
			{
				sem_p(semid,2);
				printf("This is forth child progress\n");
				sem_v(semid,4);   
			}
			else
			{
				if(0==fork())
				{
					sem_p(semid,3);
					sem_v(semid,4);
					sem_p(semid,4);
					printf("This is fifth progress\n");
					//printf("______");
				//	del_sem(semid);
				} else {
					int n = 0;
					while ( n < 4 ) {
						if ( waitpid( -1, NULL, 0 ) > 0 ) {
							n ++;
						}					
					}
				}
			}
		}	
		
	}
}		
