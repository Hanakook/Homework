#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <limits.h>  
#include <string.h>  

#include <fcntl.h> 

//��ʼ�����������ڷ�������׼����
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

    //�жϹܵ��Ƿ���ڣ���������ھʹ��������ܵ�
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

    //�������ܵ�������pipe1����A����pipe2����Aд���������򿪣�
    pipe_fd1 = open("pipe1",O_WRONLY | O_NONBLOCK);
    pipe_fd2 = open("pipe2",O_RDONLY | O_NONBLOCK);

    Init();

    //��ѭ������������A���������A�򿪵ڶ���ͨ�Źܵ�
    while(1)
    {
        //д�����
        if (gets(write_buf) != NULL)
        {
            if(strcmp(write_buf,"quit")==0)
              return 0; 
            printf("�㷢��:%s.\n",write_buf);
            write(pipe_fd1,write_buf,sizeof(write_buf));
            sleep(1);
            break;
        }
    }


    //��A��ʽͨ��
    while(1)
    {
        //��������
        read_cou = read(pipe_fd2,read_buf,PIPE_BUF);
        if(read_cou > 0)
        {
            if(strcmp(read_buf,"quit")==0)
              return 0; 
            printf("���յ�:%s\n",read_buf);
        }

        //д�����
        if (gets(write_buf) != NULL)
        {
            //�ӱ�׼�����ж�ȡ�������������д��ܵ�
            if(strcmp(write_buf,"quit")==0)
              return 0;
            printf("�㷢��:%s.\n",write_buf);
            write(pipe_fd1,write_buf,sizeof(write_buf));
        }   

    }

    return 0;
}
