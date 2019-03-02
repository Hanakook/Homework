/*
    ID = letter letter*
    NUM = digit digit*
    letter = a|..|z|A|..|Z
    digit = 0|..|9

  1  ID      字母 
  2  NUM     数字
  3  OP      操作符  
  4  FUCN    函数
  5  RESW    标识符
  6  ERROR   字母和数字以外的其他字符
  7  COM     判断注释，如果为注释就直接输出

*/



#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<iostream>
#define intputfile "test.c"      
#define outputfile "target.c"    
typedef int sInt;
using namespace std;

FILE *source;
FILE *target;

long beginp[5];   //记录文件指针的开始位置
long endp[5];    //记录文件指针的结束位置
char idStr[80]="";   //保存标识符中间值
int state[5]={0}; //不同记号类型的状态
char unaryOP[16]={'+','-','*','/','<','>','=',';',',','(',')','[',']','{','}','!'}; //保存一元运算符
char *p[6]={"if","else","int","return","void","while"};  //系统保留字
char *strToken[7]={"ID","NUM","OP","FUCN","RESW","ERROR","COM"};
typedef enum {ID,NUM,OP,FUCN,RESW,ERROR,COM}tokenType; //记号类型

//初始化
void clearState(){
    memset(state,0,sizeof(state));
    memset(idStr,'\0',sizeof(idStr));
    memset(beginp,0,sizeof(beginp));
    memset(endp,0,sizeof(endp));
}

//输出
void strPrintf(long begin,long end,tokenType t){
    bool isComment = false;
    int k;
    char s[200] = "";
    long len = end-begin;
    fseek(source,-len,1);//文件指针回退
    for(int i=0;i<len;i++) s[i]=fgetc(source);
    switch(t){
        case ID:
            k=0;break;
        case NUM:
            k=1;break;
        case OP:
            k=2;break;
        case FUCN:
            k=3;break;
        case RESW:
            k=4;break;
        case ERROR:
            k=5;break;
        case COM:
            k=6;isComment=true;break;
        default:
            cout<<"输入有误！\n";
    }
    //printf("%s\n",s);
    //如果是注释的话就直接打出来
    if(isComment){
        fprintf(target,"%s\n",s);
        printf("%s\n",s);
    }
    //其他的一个类型对应一个字符
    else{
        fprintf(target,"<%s,%s>\n",strToken[k],s);
        printf("<%s,%s>\n",strToken[k],s);
    }
}

//一元运算符的写入
void unaryPrintf(char s,tokenType t){
    int k;
    switch(t){
        case ID:
            k=0;break;
        case NUM:
            k=1;break;
        case OP:
            k=2;break;
        case FUCN:
            k=3;break;
        case RESW:
            k=4;break;
        case ERROR:
            k=5;break;
        case COM:
            k=6;break;
        default:
            cout<<"输入有误！\n";
    }
    //cout<<strToken[k]<<endl;
    fprintf(target,"<%s,%c>\n",strToken[k],s);
    printf("<%s,%c>\n",strToken[k],s);
}

//判断操作符
bool isOperator(char ch){
    for(int i=0;i<16;i++){
        if( ch==unaryOP[i] )
        return true;
    }
    return false;
}

//判断数字
void number(char ch,int i){
    beginp[i]=ftell(source)-1;
    while(!feof(source)){
        if(ch>='0' && ch<='9'){
            state[i]=1;
            endp[i]=ftell(source);
        }
        else{
            fseek(source,-1L,1); //回退,读到了下一个字符
            strPrintf(beginp[i],endp[i],NUM);
            return;
        }
        ch = fgetc(source);
    }
}

//判断注释 /**/ //
void comment(char ch,int i){
    bool isexit = false; //当不是注释时用于跳出循环
    while(!feof(source)){
        switch(state[i]){
            case 0:
                if(ch=='/'){
                    state[i]=1;
                    beginp[i]=ftell(source)-1;
                }
                break;
            case 1:
                if(ch=='*'){
                    state[i]=2;
                }
                else{
                    if(ch == '/'){
                        state[i]=5;
                    }
                    else{
                        if(ch == '='){
                            isexit = true;
                            endp[i]=ftell(source);
                            strPrintf(beginp[i],endp[i],OP);
                            
                        }
                        else{
                            isexit = true; //说明不是注释，请求退出
                            unaryPrintf('/',OP);
                            fseek(source,-1L,1); //回退一个字节，因为向后判断移了一位                            
                        }

                    }
                }
                break;
            case 2:
                if(ch=='*'){
                    state[i]=3;
                }
                else{
                    state[i]=2;
                }
                break;
            case 3:
                if(ch=='/'){
                    state[i]=4;
                    endp[i]=ftell(source);
                    strPrintf( beginp[i],endp[i],COM);  //如果是完整的注释
                    isexit = true;                               //back
                }else state[i]=2;
                break;
            case 5:
                if(ch == '\n'){
                    state[i]=4;
                    endp[i]=ftell(source);
                    strPrintf( beginp[i],endp[i],COM);  //如果是完整的注释
                    isexit = true; 
                }
                else{
                    state[i] = 5;
                }
        }
        if(isexit) return;//back;
        ch = fgetc(source);
    }
}

//判断操作符
void myOperator(char ch,int i){
    bool isReturn = false;
    while(!feof(source)){
        if(ch==';'||ch==','||ch=='('||ch==')'||ch=='['||ch==']'||ch=='{'||ch=='}'){
            state[i]=1;
            unaryPrintf(ch,OP);
            isReturn = true;
        }
        else{
            switch(state[i]){
                case 0:
                    beginp[i]=ftell(source)-1;
                    switch(ch){                      //判断四个双目运算符
                        case '<':
                            state[i]=2;break;
                        case '>':
                            state[i]=4;break;
                        case '=':
                            state[i]=6;break;
                        case '!':
                            state[i]=8;break;
                        case '+':
                            state[i]=10;break;
                        case '-':
                            state[i]=12;break;
                        case '*':
                            state[i]=14;break;
                        default:
                            cout<<"data error!\n";
                    }
                    break;
                case 2:
                    if(ch=='='){
                        state[i]=3;
                        endp[i]=ftell(source);
                        strPrintf(beginp[i],endp[i],OP);
                        isReturn = true;
                    }else{
                        //属于一元操作符
                        state[i]=2;
                        fseek(source,-1L,1);//回退一个字符
                        unaryPrintf('<',OP);
                        isReturn = true;
                    }
                    break;
                case 4:
                    if(ch=='='){
                        state[i]=5;
                        endp[i]=ftell(source);
                        strPrintf(beginp[i],endp[i],OP);
                        isReturn = true;
                    }
                    else{
                        //属于一元操作符
                        state[i]=4;
                        fseek(source,-1L,1);//回退一个字符
                        unaryPrintf('>',OP);
                        isReturn = true;
                    }
                    break;
                case 6:
                    if(ch=='='){
                        state[i]=7;
                        endp[i]=ftell(source);
                        strPrintf(beginp[i],endp[i],OP);
                        isReturn = true;
                    }
                    else{
                        //属于一元操作符
                        state[i]=6;
                        fseek(source,-1L,1);//回退一个字符
                        unaryPrintf('=',OP);
                        isReturn = true;
                    }
                    break;
                case 8:
                    if(ch=='='){
                        state[i]=9;
                        endp[i]=ftell(source);
                        strPrintf(beginp[i],endp[i],OP);
                        isReturn = true;
                    }
                    else{
                        //属于一元操作符
                        state[i]=8;
                        fseek(source,-1L,1);//回退一个字符
                        unaryPrintf('!',OP);
                        isReturn = true;
                    }
                    break;
                case 10:
                    if(ch=='='){
                        state[i]=11;
                        endp[i]=ftell(source);
                        strPrintf(beginp[i],endp[i],OP);
                        isReturn = true;
                    }
                    else if(ch == '+'){
                        state[i]=11;
                        endp[i]=ftell(source);
                        strPrintf(beginp[i],endp[i],OP);
                        isReturn = true;                        
                    }
                    else{
                        //属于一元操作符
                        state[i]=10;
                        fseek(source,-1L,1);//回退一个字符
                        unaryPrintf('+',OP);
                        isReturn = true;
                    }
                    break;
                case 12:
                    if(ch=='='){
                        state[i]=13;
                        endp[i]=ftell(source);
                        strPrintf(beginp[i],endp[i],OP);
                        isReturn = true;
                    }
                    else if(ch == '-'){
                        state[i]=13;
                        endp[i]=ftell(source);
                        strPrintf(beginp[i],endp[i],OP);
                        isReturn = true;
                    }
                    else{
                        //属于一元操作符
                        state[i]=12;
                        fseek(source,-1L,1);//回退一个字符
                        unaryPrintf('-',OP);
                        isReturn = true;
                    }
                    break;
                case 14:
                    if(ch=='='){
                        state[i]=15;
                        endp[i]=ftell(source);
                        strPrintf(beginp[i],endp[i],OP);
                        isReturn = true;
                    }
                    else{
                        //属于一元操作符
                        state[i]=14;
                        fseek(source,-1L,1);//回退一个字符
                        unaryPrintf('*',OP);
                        isReturn = true;
                    }
                    break;
                default:
                    cout<<"data error!\n";
            }
        }
        if(isReturn) return;
        ch = fgetc(source);
    }
}

//判断字母
bool isLiter(char ch){
    if((ch>='A' && ch<='Z')||( ch>= 'a' && ch<='z') || ch=='_'){
        return true;
    }
    else
        return false;
}

//判断关键字
bool isResw(char *s){
    for(int i=0;i<6;i++){
        if( strcmp(s,p[i])==0 )
        return true;
    }
    return false;
}

void identifier(char ch,int i){
    beginp[i] = ftell(source)-1;
    long len =0;
    bool isQuit = false;
    bool isFucn = false;
    while(!feof(source)){
        if( isLiter(ch) ){
            state[i]=1;
            endp[i]=ftell(source);
        }
        else{
            long enter = 1; //
            isQuit = true;
            if(ch=='\n') enter = 2;
            fseek(source,-enter,1); //回退一或2个字符
            //cout<<"pos="<<ftell(source)<<endl;
            //printf("%d %d\n",beginp[i],endp[i]);
            len = endp[i]-beginp[i];
            fseek(source,-len,1);
            //cout<<"pos="<<ftell(source)<<endl;
            for(int j=0;j<len;j++){
                idStr[j] = fgetc(source);

            }
            if( isResw(idStr) ){    //如果是保留字，就保存退出
                strPrintf(beginp[i],endp[i],RESW);

            }
            else{
                char temps;
                long cout=1;
                temps = fgetc(source);
                while(!feof(source)){
                    if(temps==' ' ||temps=='\n' || temps=='\t'){
                        ; //jump it;
                    }
                    else{
                        if(temps=='('){
                            isFucn = true; //表明是函数名
                            break;
                        }
                        else{
                            isFucn = false; //不是函数名
                            break;
                        }
                    }
                    temps = fgetc(source);
                    cout++;
                }
                fseek(source,-cout,1); //回退文件指针
                //printf("back=%ld\n",ftell(source));
                if(isFucn){
                    strPrintf(beginp[i],endp[i],FUCN);
                }
                else{
                    strPrintf(beginp[i],endp[i],ID);
                }
            }
        }
        if(isQuit) return;
        ch = fgetc(source);
    }
}

int flag = 0;
int cnt = 0;
//扫描文件
void Scanner(){
    char ch=fgetc(source);
    while( !feof(source) ){
        if(ch==' ' ||ch=='\t'){
            ;//nothing jump it!
        }
        else if(ch == '\n'){  
            if(flag){
                flag = 0;
                comment(ch,0);
                clearState(); //清楚状态信息

            }
            else{
                ;
            }
        }
        else if(ch=='/'){
            cnt++;
            comment(ch,0);
            clearState(); //清楚状态信息
        }
        else if(ch>='0'&&ch<='9'){
            number(ch,1); //处理数字
            clearState();
        }
        else if( isOperator(ch) ){
            myOperator(ch,2); //处理操作符
            clearState();
        }
        else if( isLiter(ch) ){
            identifier(ch,3); //处理标志符
            clearState();
        }
        else{
            unaryPrintf(ch,ERROR);
        }
        ch = fgetc(source);
    }
}

bool read(){
    if(((source=fopen(intputfile,"r"))==NULL)||((target=fopen(outputfile,"w"))==NULL))
        return 0;
    return 1;
}


int main(){
    if(!read()){
        printf("文件打开失败!");
        exit(0);
    }
    Scanner();  //开始扫描文件 entrance
    fclose(source);
    fclose(target);
    //printf("%d",cnt);
    return 0;
}
