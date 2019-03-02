#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>

char prog[80];//存放所有的输入字符
char token[8];//存放词组
char ch;//单个字符
int syn,p,m,n,i;
double sum;
int count;

int isSignal;//判断符号
int isError;
int isDecimal;//是否小数
double decimal;
int isExp;//指数
int Index;//指数幂
int isNegative;//是否负号

double temp;
int temp2;
int repeat;//连续出现+-*
int nextq;
int kk;//临时变量的标号
int ntc,nfc,nnc,nnb,nna;

//关键字
char *rwtab[9]={"main","int","float","double","char","if","else","do","while"};

//四元组结构体定义
struct{
    char result[10];//字符串
    char arg1[10];
    char opera[10];
    char arg2[10];
}fourCom[20];

void scanner();//扫描分析单词
void lrparser();
void staBlock(int *nChain);//语句块分析函数
void staString(int *nChain);//语句串分析函数
void sta(int *nChain);//语句分析函数
void fuzhi();//赋值语句分析函数
void tiaojian(int *nChain);//条件语句分析函数
void xunhuan();//循环语句分析函数
char* E();//表达式
char* T();//项
char* F();//因子
char *newTemp();//自动生成临时变量
void backpatch(int p,int t);//进行回溯回填
int merge(int p1,int p2);//合并p1p2
void emit(char *res,char *num1,char *op,char *num2);//生成四元式

int main()
{
    p=0;
    count=0;
    isDecimal=0;
    Index=0;
    repeat=0;
    
    kk=0;
    
    //输入源程序，以#结尾。
    printf("\n请输入待编译程序:\n");
    do{
        ch=getchar();
        prog[p++]=ch;
    }while(ch!='#');
    p=0;
    isError=0;
    scanner();
    lrparser();
    //输出四元式
    for(i=1;i<nextq;i++)
    {
        printf("\n%d\t",i);
        printf("(%5s %5s %5s \t%5s   )\n",fourCom[i].arg1,fourCom[i].opera,fourCom[i].arg2,fourCom[i].result);
    }
    return 0;
}


void lrparser()
{
    int nChain;
    nfc=ntc=1;
    nextq=1;

    if(syn==1)//main函数
    {
        scanner();
        if(syn==26)//判断(
        {
            scanner();
            if(syn==27)//判断至）
            {
                scanner();
                staBlock(&nChain);
            }
            else
                printf("缺少右括号\n");
        }
        else
            printf("缺少左括号\n");
    }
    else
        printf("缺少main\n");
}

/* <语句块> ::= '{'<语句串>'}'*/
//语句块分析函数
void staBlock(int *nChain)
{
    //分析{}
    if(syn==28)//检测{
    {
        scanner();
        staString(nChain);  //扫描整个语句
        if(syn==29)//扫描至}读取下一个
            scanner();
        else//没有扫描到}
            printf("缺少}号\n");
    }
    else{//语句块没有{
        printf("缺少{号\n");
    }
}

/* <语句串> ::= <语句>{;<语句>}*/
//语句串分析函数
void staString(int *nChain)
{
    //语句分析
    sta(nChain);
    backpatch(*nChain,nextq);
    while(syn==31)//检测到；分析该句再进行下一句。
    {
        scanner();
        sta(nChain);
    }
}

//语句分析函数
void sta(int *nChain)
{
    if(syn==10)//赋值语句,
    {
        fuzhi();
    }
    else if(syn==6)//条件语句
    {
        tiaojian(nChain);
    }
    else if(syn==8)//循环语句
        xunhuan();
}

/*<条件语句>::=if<条件><语句块>*/  
//条件分析函数
void tiaojian(int *nChain)
{
    char res[10],num1[10],num2[10],op[10];
    int nChainTemp;
    if(syn==6)//检测if
    {
        scanner();
        if(syn==26)//括号内的条件分析
        {
            scanner();
            strcpy(num1,E());
            if((syn<=37)&&(syn>=32))//比较运算符
            {
                switch(syn)
                {
                    case 32:
                        strcpy(op,">");
                        break;
                    case 33:
                        strcpy(op,">=");
                        break;
                    case 34:
                        strcpy(op,"<");
                        break;
                    case 35:
                        strcpy(op,"<=");
                        break;
                    case 36:
                        strcpy(op,"==");
                        break;
                    case 37:
                        strcpy(op,"!=");
                        break;
                    default:
                        printf("error");
                }
            }
            scanner();
            strcpy(num2,E());
            strcat(num1,op);
            strcat(num1,num2);

            ntc=nextq;//记住if位置
            emit("0","if",num1,"goto");
            nfc=nextq;//表达式为假
            emit("0","","","goto");
            backpatch(ntc,nextq);
        }
        if(syn==27)
            scanner();
        staBlock(&nChainTemp);
        *nChain=merge(nChainTemp,nfc);//链接所有四元式回填nextq
    }
}

/*<循环语句>::=do <语句块>while <条件>*/
//循环语句分析
void xunhuan()
{
    char res[10],num1[10],num2[10],op[10];
    int nChainTemp;
    
    if(syn==8)//判断do
    {
        nnc=nextq;//记住if位置
        scanner();
        staBlock(&nChainTemp);
        if(syn==9)//判断至while
        {
            scanner();
            if(syn==26)//分析while内的条件语句。
            {
                scanner();
                strcpy(num1,E());
                if((syn<=37)&&(syn>=32))
                {
                    switch(syn)
                    {
                        case 32:
                            strcpy(op,">");
                            break;
                        case 33:
                            strcpy(op,">=");
                            break;
                        case 34:
                            strcpy(op,"<");
                            break;
                        case 35:
                            strcpy(op,"<=");
                            break;
                        case 36:
                            strcpy(op,"==");
                            break;
                        case 37:
                            strcpy(op,"!=");
                            break;
                        default:
                            printf("error");
                    }
                }
                scanner();
                strcpy(num2,E());
                strcat(num1,op);
                strcat(num1,num2);
                nnb=nextq;
                emit("0","if",num1,"goto");
                backpatch(nnb,nnc);
                nna=nextq;
                emit("0","","","goto");
                backpatch(nna,nextq);
            }
            if(syn==27)//判断至)
                scanner();
        }
    }
}

//赋值语句分析函数
void fuzhi()//只有一个操作数
{
    char res[10],num[10];
    
    if(syn==10)//内部字符串
    {
        strcpy(res,token);//结果
        scanner();
        if(syn==21)//判断=
        {
            scanner();
            strcpy(num,E());
            emit(res,num,"=","");
        }
        else
        {
            printf("缺少=号\n");
        }
    }
}

//表达式
char* E()
{
    char *res,*num1,*op,*num2;
    res=(char *)malloc(10);
    num1=(char *)malloc(10);
    op=(char *)malloc(10);
    num2=(char *)malloc(10);
    strcpy(num1,T());
    while((syn==22)||(syn==23))//+-
    {
        if(syn==22)
            strcpy(op,"+");
        else
            strcpy(op,"-");
        scanner();
        strcpy(num2,T());
        strcpy(res,newTemp());
        emit(res,num1,op,num2);
        strcpy(num1,res);
    }
    return num1;
}

//项
char* T()
{
    char *res,*num1,*op,*num2;
    res=(char *)malloc(10);
    num1=(char *)malloc(10);
    op=(char *)malloc(10);
    num2=(char *)malloc(10);
    strcpy(num1,F());
    while((syn==24)||(syn==25)) //* /
    {
        if(syn==24)
            strcpy(op,"*");
        else
            strcpy(op,"/");
        scanner();
        strcpy(num2,F());
        strcpy(res,newTemp());
        emit(res,num1,op,num2);
        strcpy(num1,res);
    }
    return num1;
}

//因子
char* F()
{
    char *res;
    res=(char *)malloc(10);
    if(syn==10)//字符串
    {
        strcpy(res,token);
        scanner();
    }
    else if(syn==20)//二进制
    {
        //itoa((int)sum,res,10);
        sprintf(res, "%d", (int)sum);//整数转成字符串
        scanner();
    }
    else if(syn==26)
    {
        scanner();
        res=E();
        if(syn==27)
        {
            scanner();
        }
        else isError=1;
    }
    else
        isError=1;
    return res;
}

//临时变量
char *newTemp()
{
    char *p;
    char varTemp[10];
    p=(char *)malloc(10);
    kk++;
    // itoa(kk,varTemp,10);
    sprintf(varTemp, "%d", kk);
    strcpy(p+1,varTemp);
    p[0]='T';
    return p;
}

//回填。p链接每个四元式的第四个分量回填t
void backpatch(int p,int t)
{
    int w,circle=p;
    while(circle)//不为0
    {
        w=atoi(fourCom[circle].result);//第四分量内容。
        sprintf(fourCom[circle].result,"%d",t);//将t填入
        circle=w;
    }
    return;
}

//合并
int merge(int p1,int p2)
{
    char circle,nResult;
    if(p2==0)
        nResult=p1;
    else
    {
        nResult=circle=p2;
        while(atoi(fourCom[circle].result))
        {
            circle=atoi(fourCom[circle].result);//第四分量不为0
            sprintf(fourCom[circle].result,"%s",p1);
        }
    }
    return nResult;
}

//调出四元式的结果，运算符和两变量
void emit(char *res,char *num1,char *op,char *num2)
{
    strcpy(fourCom[nextq].result,res);
    strcpy(fourCom[nextq].arg1,num1);
    strcpy(fourCom[nextq].opera,op);
    strcpy(fourCom[nextq].arg2,num2);
    nextq++;
}

// 扫描
void scanner()
{
    sum=0;
    decimal=0;
    m=0;
    for(n=0;n<8;n++)
        token[n]=NULL;
    ch=prog[p++];
    //空格回车忽略
    while(ch==' '||ch=='\n')
        ch=prog[p++];

    //判断字母
    if(((ch>='a')&&(ch<='z'))||((ch>='A')&&(ch<='Z')))
    {
        while(((ch>='a')&&(ch<='z'))||((ch>='A')&&(ch<='Z'))||((ch>='0')&&(ch<='9')))
        {
            token[m++]=ch;
            ch=prog[p++];
        }
        token[m++]='\0';
        p--;
        syn=10;
        for(n=0;n<9;n++)
            if(strcmp(token,rwtab[n])==0)
            {
                syn=n+1;
                break;
            }
    }
    else if((ch>='0')&&(ch<='9'))//判断数字，int double类型
    {
    IsNum:
        if(isSignal==1)
        {}
        while((ch>='0')&&(ch<='9'))
        {
            sum=sum*10+ch-'0';
            ch=prog[p++];
        }
        if(ch=='.')//小数点
        {
            isDecimal=1;
            ch=prog[p++];
            count=0;
            while((ch>='0')&&(ch<='9'))
            {
                temp=(ch-'0')*pow(0.1,++count);
                decimal=decimal+temp;
                ch=prog[p++];
            }
            sum=sum+decimal;
        }
        if(ch=='e'||ch=='E')//科学计数法
        {
            isExp=1;
            ch=prog[p++];
            if(ch=='-')
            {
                isNegative=1;
                ch=prog[p++];
            }
            while((ch>='0')&&(ch<='9'))
            {
                Index=Index*10+ch-'0';
                ch=prog[p++];
            }
            if(isNegative)
                sum=sum*pow(0.1,Index);
            else
                sum=sum*pow(10,Index);
            
        }
        if(isSignal==1)//符号位
        {
            sum=-sum;
            isSignal=0;
        }
        p--;
        syn=20;
    }
    else switch(ch)//运算符判断
    {
        case '<':
            m=0;
            token[m++]=ch;
            ch=prog[p++];
            if(ch=='=')
            {
                syn=35;
                token[m++]=ch;
            }
            else
            {
                syn=34;
                p--;
            }
            break;
            
        case '>':
            m=0;
            token[m++]=ch;
            ch=prog[p++];
            if(ch=='=') 
            {
                syn=33;
                token[m++]=ch;
            }
            else
            {
                syn=32;
                p--;
            }
            break;
            
        case '=':
            m=0;
            token[m++]=ch;
            ch=prog[p++];
            if(ch=='=')
            {
                syn=36;
                token[m++]=ch;
            }
            else
            {
                syn=21;
                p--;
            }
            break;
        case '+':
            temp2=prog[p];
            token[m++]=ch;
            if((temp2>='0')&&(temp2<='9')&&(repeat==1))
            {
                isSignal=2;
                ch=prog[p++];
                repeat=0;
                goto IsNum;
            }
            if(((temp2=='+')||(temp2=='-'))&&(repeat==0))
            {
                repeat=1;
            }            syn=22;
            break;
        case '-':
            temp2=prog[p];
            token[m++]=ch;
            if((temp2>='0')&&(temp2<='9')&&(repeat==1))
            {
                isSignal=1;
                ch=prog[p++];
                repeat=0;
                goto IsNum;
            }
            if(((temp2=='+')||(temp2=='-'))&&(repeat==0))
            {
                repeat=1;
            }
            syn=23;
            break;
        case '*':
            temp2=prog[p];
            token[m++]=ch;
            
            if(temp2=='+')
            {
                isSignal=2;
                repeat=1;
            }
            else if(temp2=='-')
            {
                isSignal=1;
                repeat=1;
            }
            syn=24;
            break;
            
        case '/':
            syn=25;
            token[m++]=ch;
            break;
        case '(':
            temp2=prog[p];
            token[m++]=ch;
            
            if(temp2=='+')
            {
                isSignal=2;
                repeat=1;
            }
            else if(temp2=='-')
            {
                isSignal=1;
                repeat=1;
            }
            syn=26;
            break;
        case ')':
            syn=27;
            token[m++]=ch;
            break;
        case '{':
            syn=28;
            token[m++]=ch;
            break;
        case '}':
            syn=29;
            token[m++]=ch;
            break;
        case ',':
            syn=30;
            token[m++]=ch;
            break;
        case ';':
            syn=31;
            token[m++]=ch;
            break;
        case'#':
            syn=0;
            token[m++]=ch;
            break;
        default:
            syn=-1;
    }
}


