#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <limits.h>  
#include <string.h>  

#include <fcntl.h> 

//初始化函数，用于非阻塞标准输入
int Init()
{
    if(-1 == fcntl(0,F_SETFL,O_NONBLOCK))
    {
        printf("fail to change the  std mode.\n");
        return -1;
    }


}

int main()
{
    int pipe_fd1,pipe_fd2;
    int read_cou = 0,write_cou = 0;
    char read_buf[100];
    char write_buf[100];
    memset(read_buf,'\0',sizeof(read_buf));
    memset(write_buf,'\0',sizeof(write_buf));

    //判断管道是否存在，如果不存在就创建有名管道
    if(-1 == access("pipe1",F_OK))
    {
        if(-1 == mkfifo("pipe1",0777))
        {
            printf("Could not create pipe1\n");
            return -1;
        }
    }

    if(-1 == access("pipe2",F_OK))
    {
        if(-1 == mkfifo("pipe2",0777))
        {
            printf("Could not create pipe2\n");
            return -1;
        }
    }

    //打开两个管道，其中pipe1用于A读，pipe2用于A写（非阻塞打开）
    pipe_fd1 = open("pipe1",O_WRONLY | O_NONBLOCK);
    pipe_fd2 = open("pipe2",O_RDONLY | O_NONBLOCK);

    Init();

    //此循环用于首先向A提出请求，是A打开第二个通信管道
    while(1)
    {
        //写入过程
        if (gets(write_buf) != NULL)
        {
            if(strcmp(write_buf,"quit")==0)
              return 0; 
            printf("你发出:%s.\n",write_buf);
            write(pipe_fd1,write_buf,sizeof(write_buf));
            sleep(1);
            break;
        }
    }


    //和A正式通信
    while(1)
    {
        //读出过程
        read_cou = read(pipe_fd2,read_buf,PIPE_BUF);
        if(read_cou > 0)
        {
            if(strcmp(read_buf,"quit")==0)
              return 0; 
            printf("你收到:%s\n",read_buf);
        }

        //写入过程
        if (gets(write_buf) != NULL)
        {
            //从标准输入中读取，如果有输入再写入管道
            if(strcmp(write_buf,"quit")==0)
              return 0;
            printf("你发出:%s.\n",write_buf);
            write(pipe_fd1,write_buf,sizeof(write_buf));
        }   

    }

    return 0;
}
