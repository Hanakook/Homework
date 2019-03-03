#include<stdio.h>
#include<sys/types.h>
#include<time.h>
#include<stdlib.h>
#include<unistd.h>
static double allsum=0.0;
int main()
{
	int i,j;
	double a[4][25000];   //用来存放100000个随机数
	double sum[4]={0.0};   //用来存放四组随机数各自的和
	double readbuf[4];     //来用存放从管道中读取的数据
	pid_t pid;
	int fd[4][2];
	srand((unsigned)time(NULL));//设置随机数种子
	for(i=0;i<4;i++)
		for(j=0;j<25000;j++)
			a[i][j]=(double)rand()/(double)RAND_MAX;//获得0-1之间的浮点随机数
	for(i=0;i<4;i++)//创建4个管道和4个子进程
	{
		if(pipe(fd[i])<0)
		{
			printf("pipe error");
			exit(1);
		}
		pid=fork();
		if(pid<0)
		{
			printf("fork error!");
			exit(1);
		}
		else if(pid==0)//子进程进行求和操作
		{
		   for(j=0;j<25000;j++)
			   sum[i]+=a[i][j];
		   write(fd[i][1],&sum[i],sizeof(double));
		  // printf("sum[%d]==%f\n",i,sum[i]);
		   exit(1);
		}
		else
		{
			if(!read(fd[i][0],&readbuf[i],sizeof(double)))//父进程进行读取操作
			{
				exit(1);
			}
			printf("sum[%d]=%f\n",i,readbuf[i]);
		}
	}
		for(i=0;i<4;i++)
			allsum+=readbuf[i];
		printf("sum=%f\n",allsum);
		printf("average=%f\n",allsum/100000);
		return 0;
}

