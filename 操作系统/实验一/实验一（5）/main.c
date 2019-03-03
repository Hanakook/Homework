#include <stdio.h>
#define Mem_Num 3
int Int_Count = 0; 
typedef struct Stack
{
	int cur;
	int value[Mem_Num];
}Stack; 
int find_Stack(Stack *s1,int value)
{
	for(int i=0;i<=s1->cur;++i)
	{
		if(s1->value[i]==value) 
		  return i;
	}
     return -1;
} 
int first_add_Stack(Stack *s1,int value)
{
	int pos;
	int i;
	if((pos=find_Stack(s1,value))==-1)
	{
		printf("did not find in mem_Stack!\n");
		Int_Count++;
		if(s1->cur==Mem_Num-1)
		{
			for(i=0;i<=s1->cur-1;++i)
			s1->value[i]=s1->value[i+1];
		    s1->value[i]=value;
		    return 0;
		}
		else if(s1->cur<Mem_Num-1)
		{
			s1->cur++;
			s1->value[s1->cur]=value;
			return 1;
		}
		else
		{
			printf("wrong stack~\n");
			return 2;
		}
	}
	else
	{
		printf("find in stack!\n");
		return 3;
	}
}
int new_add_Stack(Stack *s1,int value)
{
    int pos;
	int i;
	if((pos=find_Stack(s1,value))==-1)
	
		printf("did not find in mem_Stack!\n");
		Int_Count++;
		if(s1->cur==Mem_Num-1)
		{
			for(i=0;i<=s1->cur-1;++i)
			s1->value[i]=s1->value[i+1]