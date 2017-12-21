#include "OS.h"

/*--------------全局变量-------------------------*/
extern char* myvhard;//虚拟磁盘起始地址
extern string currentdir;//当前目录
extern string cmd; //读取指令
extern USEROPEN openfilelist[MAXOPENFILE];//文件打开表
extern USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
extern DISK* disk;//将内容结构化
extern char command[50];//文件名标示符

/*-----------------------------打开文件函数--------------------*/
int open(char *filename){

    //检查要被打开文件是否存在
    int BlockDirNum = (ptrcuridr->fcb).first;
    DirFile *dir = (DirFile *) disk->Data[BlockDirNum - 8];
    int Fileaddr = -1;
    for (int i = 0; i < FCBCOUNT; i++) {
        if (dir->fcb[i].free == 1 && strcmp(dir->fcb[i].filename, filename) == 0)//表项被使用,且是目录,且文件名相等
        {
            Fileaddr = i;//文件存在
            break;
        }
    }
    //文件不存在 输出-1
    if(Fileaddr==-1){
        printf("file does not exist\n");
        return -1;
    }

    //检查打开文件表是否还有空表项,没有报错，有则记录
    int OpenFileaddr=-1;
    for(int i=0;i<MAXOPENFILE;i++) {
        if (openfilelist[i].topenfile == 0) {
            OpenFileaddr=i;
        }
    }
    //没有空表了
    if(OpenFileaddr==-1) {
        printf("File open table is full \n");
        return -1;
    }
    //如果又要打开一个根目录,那么直接返回0
    if(dir->fcb[Fileaddr].first==8)
    {
        OpenFileaddr=0;
        if(ptrcuridr->fcb.first==8)
            return 0;
    }
    //为该文件填写文件打开表项

    //检查是否已经打开
    //需要一个temp来表示实际的dir值
    char temp[300];
    if(strcmp(filename,"..")==0)
    {
        strcpy(temp,ptrcuridr->dir);
        int len1=strlen(ptrcuridr->dir);
        int len2=strlen(ptrcuridr->fatherfilename);
        temp[len1-len2-1]=0;
    }
    else
    {
        char buffer[80];
        memset(buffer,0,sizeof(buffer));
        strcat(strcat(strcat(buffer,ptrcuridr->dir),"/"),ptrcuridr->fcb.filename);
        strcpy(temp,buffer);
    }
    for(int i=1;i<MAXOPENFILE;i++) {
        //"."一定是被打开了
        if ((openfilelist[i].topenfile == 1 && strcmp(openfilelist[i].fcb.filename, filename) == 0 &&
             strcmp(openfilelist[i].dir,temp) ==0 )||(strcmp(filename,".")==0))
             {
           // printf(" The file has been opened !\n");
            return -1;//无效返回-1
        }
    }
    if(strcmp(filename,"..")==0)
    {
        openfilelist[OpenFileaddr].fcb=dir->fcb[Fileaddr];
        //名字是错的,会是"..";正确的名字在工作块的父亲名字
        strcpy(openfilelist[OpenFileaddr].fcb.filename,ptrcuridr->fatherfilename);
        //曾经的路径减去名字减去'/' 就是新的
        strcpy(openfilelist[OpenFileaddr].dir,ptrcuridr->dir);
        int len1=strlen(openfilelist[OpenFileaddr].dir);
        int len2=strlen(ptrcuridr->fatherfilename);
        openfilelist[OpenFileaddr].dir[len1-len2-1]=0;
        //找新的fathername,通过分析dir来得到
        char test[20];
        strcpy(test,openfilelist[OpenFileaddr].dir);
        char *q=NULLc;
        int len=strlen(test);
        for(int i=0;i<len;i++)
        {
            if(test[i]=='/'&&i!=len-1)
                q=test+i+1;
        }
        if(q!=NULL)
        strcpy(openfilelist[OpenFileaddr].fatherfilename,q);
    }
    else
    {
        openfilelist[OpenFileaddr].fcb=dir->fcb[Fileaddr];
        char buffer[80];
        memset(buffer,0,sizeof(buffer));
        strcat(strcat(strcat(buffer,ptrcuridr->dir),"/"),ptrcuridr->fcb.filename);
        strcpy(openfilelist[OpenFileaddr].dir,buffer);
        strcpy(openfilelist[OpenFileaddr].fatherfilename,ptrcuridr->fcb.filename);
    }
    openfilelist[OpenFileaddr].pos=OpenFileaddr;
    openfilelist[OpenFileaddr].count=0;
    openfilelist[OpenFileaddr].fcbstate=0;
    openfilelist[OpenFileaddr].topenfile=1;
    //返回文件描述符fd，此时的fd跟下标相同,一般不同.
    if(openfilelist[OpenFileaddr].fcb.attribute==0)//如果是文件,输出文件打开符号
        printf("File Open Success,The fd is %d\n",OpenFileaddr);
    return OpenFileaddr;
}
