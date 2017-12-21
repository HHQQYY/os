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
/*--------------------------删除文件函数---------------------*/
int rm(char *filename){
/*--------------------------打开上级目录---------------------*/
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
            filename = newdir;
        }
        else {
            printf("error\n");
            return -1;
        }
    }
    //调用read,判断目录下文件是否存在
    int BlockDirNum = (ptrcuridr->fcb).first;
    DirFile *dir = (DirFile *) disk->Data[BlockDirNum - 8];
    int temp = -1;
    for (int i = 0; i < FCBCOUNT; i++) {
        if (dir->fcb[i].free == 1 && dir->fcb[i].attribute == 0 &&
            strcmp(dir->fcb[i].filename, filename) == 0)//表项被使用,且是文件,且文件名相等
        {
            temp = i;//文件存在
            break;
        }
    }
    //要删除的目录不存在
    if(temp == -1) {
        printf("rm: failed to remove '%s': No such file or directory\n",filename);
        if(fd!=-1)
            close(fd);
        ptrcuridr=tempp;
        return 0;
    }
    //检查该文件是否打开,若打开则关闭
    char buffer[80];
    memset(buffer,0,sizeof(buffer));
    strcat(strcat(strcat(buffer,ptrcuridr->dir),"/"),ptrcuridr->fcb.filename);
    for(int i=1;i<MAXOPENFILE;i++) {
        //"."一定是被打开了
        if ((openfilelist[i].topenfile == 1 && strcmp(openfilelist[i].fcb.filename, filename) == 0 &&
             strcmp(openfilelist[i].dir,buffer) ==0 ))
        {
            printf(" The file  been opened,Now Close it !\n");
            close(i);
            break;
        }
    }
    //回收磁盘,一个链表
    int TEMP=0;
    for(int p=dir->fcb[temp].first;p!=-1;p=TEMP)
    {
        TEMP=disk->FAT1[p];
        disk->FAT1[p]=disk->FAT2[p]=0;
    }
    //清空该目录项,free字段为0,
    memset(&(dir->fcb[temp]),0,sizeof(dir->fcb[temp]));
    //修改长度,表项的fcbstate置为1
    ptrcuridr->fcbstate=1;
    ptrcuridr->fcb.length--;//不包括.和..的
    /*-----------------恢复现场-------------*/
    if(fd!=-1)
        close(fd);
    ptrcuridr=tempp;
    return 1;
}