#include "OS.h"
int close(int fd);
int open_path(char *dirname);
int FileSubstr(char *str);
/*--------------全局变量-------------------------*/
extern char* myvhard;//虚拟磁盘起始地址
extern string currentdir;//当前目录
extern string cmd; //读取指令
extern USEROPEN openfilelist[MAXOPENFILE];//文件打开表
extern USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
extern DISK* disk;//将内容结构化
extern char command[50];//文件名标示符


int creat(char *filename){

    /*-----------打开路径所指的父目录---------*/
    char newdir[20];
    char dirpath[60];
    USEROPEN *tempp = ptrcuridr;//暂时保管一下ptrcuridr原始值,可能要回溯
    int k=FileSubstr(filename),fd=-1;
    if(k!=-1) {
        filename[k] = 0;
        memset(newdir, 0, sizeof(newdir));
        memset(dirpath, 0, sizeof(dirpath));
        strcpy(newdir, filename + k + 1);
        strcpy(dirpath, filename);
        fd = open_path(dirpath);
        if(fd!=-1) {
            ptrcuridr = openfilelist + fd;
            strcpy(filename,newdir);
        }
        else {
            printf("error\n");
            return -1;
        }
    }
    //为新文件分配一个空闲打开文件表项,如果没有空闲表项就返回 -1
    //检查打开文件表是否还有空表项,没有报错，有则记录
    int OpenFileaddr=-1;
    for(int i=0;i<MAXOPENFILE;i++) {
        if (openfilelist[i].topenfile == 0) {
            OpenFileaddr=i;
        }
    }
    //没有空表了
    if(OpenFileaddr==-1) {
        if(fd!=-1)
            close(fd);
        ptrcuridr=tempp;
        printf("File open table is full \n");
        return -1;
    }
    //检查重名
    int BlockDirNum = (ptrcuridr->fcb).first;
    DirFile *dir = (DirFile *) disk->Data[BlockDirNum - 8];
    int temp = -1,DirFreeItems =-1;
    for (int i = 0; i < FCBCOUNT; i++) {
        if (dir->fcb[i].free == 1  &&
            strcmp(dir->fcb[i].filename, filename) == 0)//表项被使用,且是目录,且文件名相等
        {
            temp = i;//有重名,可能文件,可能文件夹
            break;
        }
        else if (dir->fcb[i].free == 0) {
            DirFreeItems = i;//目录空闲表项
        }
    }
    //如果文件名已存在,报错并退出
    if(temp!=-1)
    {
        if(fd!=-1)
            close(fd);
        ptrcuridr=tempp;
        printf("creat: cannot create file '%s': File exists\n",filename);
        return 0;
    }
    //如果没有空闲位置，报错退出
    if(DirFreeItems==-1)
    {
        printf("creat: cannot create file '%s': Directory is full \n",filename);
        if(fd!=-1)
            close(fd);
        ptrcuridr=tempp;
        return 0;
    }
    //检查FAT中是否有空闲的盘块
    int FATFreeItems=-1;
    for(int i = 0;i < BLOCKCOUNT;i++)
    {
        if(disk->FAT1[i] == 0) {//没被使用的块标记为0
            FATFreeItems=i;//找到了一个空闲块
            break;
        }
    }
    //如果FAT没有空闲块,报错退出
    if(FATFreeItems==-1)
    {
        printf("mkdir: cannot create directory '%s': Disk is full \n",filename);
        if(fd!=-1)
            close(fd);
        ptrcuridr=tempp;
        return 0;
    }
    /*----------------开始新建文件-------------------*/
    //修改长度,fcbstate置为1
    ptrcuridr->fcbstate=1;
    ptrcuridr->fcb.length++;//不包括.和..的
    //准备好新文件的FCB的内容,文件属性为数据文件，长度0.
    //分配FAT的空闲块,-3表示被文件使用,-1表示文件结尾
    disk->FAT1[FATFreeItems]=disk->FAT2[FATFreeItems]=-1;
    //将改块分配到 当前目录的空闲项目下
    strcpy(dir->fcb[DirFreeItems].filename,filename);
    dir->fcb[DirFreeItems].free=1;//被使用
    dir->fcb[DirFreeItems].attribute=0;//是文件
    dir->fcb[DirFreeItems].first=FATFreeItems;//分配FAT空闲块
    dir->fcb[DirFreeItems].length=0;
    dir->fcb[DirFreeItems].data=0;//时间之后弄
    dir->fcb[DirFreeItems].time=0;//时间之后弄
    //文件新建完毕
    //填写文件打开表项
    openfilelist[OpenFileaddr].fcb=dir->fcb[DirFreeItems];
    char buffer[80];
    memset(buffer,0,sizeof(buffer));
    strcat(strcat(strcat(buffer,ptrcuridr->dir),"/"),ptrcuridr->fcb.filename);
    strcpy(openfilelist[OpenFileaddr].dir,buffer);
    strcpy(openfilelist[OpenFileaddr].fatherfilename,ptrcuridr->fcb.filename);
    openfilelist[OpenFileaddr].pos=OpenFileaddr;
    openfilelist[OpenFileaddr].count=0;
    openfilelist[OpenFileaddr].fcbstate=0;
    openfilelist[OpenFileaddr].topenfile=1;
    //关闭文件走人
    if(fd!=-1)
        close(fd);
    ptrcuridr=tempp;
    //返回文件描述符fd，此时的fd跟下标相同,一般不同.
    printf("File Creat Success,The fd is %d\n",OpenFileaddr);
    return OpenFileaddr;
}
