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
/*-------------------------------删除子目录函数---------------------*/
int rmdir(char *dirname) {

    char newdir[20];
    char dirpath[60];
    USEROPEN *tempp = ptrcuridr;//暂时保管一下ptrcuridr原始值,可能要回溯
    int k=FileSubstr(dirname),fd=-1;
    if(k!=-1) {
        dirname[k] = 0;
        memset(newdir, 0, sizeof(newdir));
        memset(dirpath, 0, sizeof(dirpath));
        strcpy(newdir, dirname + k + 1);
        strcpy(dirpath, dirname);
        fd = open_path(dirpath);
        if(fd!=-1) {
            ptrcuridr = openfilelist + fd;
            dirname = newdir;
        }
        else {
            printf("error\n");
            return -1;
        }
    }



    /*---------使用open_path更新----------------------*/
    //检查文件是否存在
    int BlockDirNum = (ptrcuridr->fcb).first;
    DirFile *dir = (DirFile *) disk->Data[BlockDirNum - 8];
    int temp = -1;
    for (int i = 0; i < FCBCOUNT; i++) {
        if (dir->fcb[i].free == 1 && dir->fcb[i].attribute == 1 &&
            strcmp(dir->fcb[i].filename, dirname) == 0)//表项被使用,且是目录,且文件名相等
        {
            temp = i;//文件存在
            break;
        }
    }
    //要删除的目录不存在
    if(temp == -1) {
        printf("rmdir: failed to remove '%s': No such file or directory\n",dirname);
        if(fd!=-1)
            close(fd);
        ptrcuridr=tempp;
        return 0;
    }
    //判断子目录是否为空,不包括0和1,也可以用length,懒得用
    DirFile *dirson=(DirFile*)(disk->Data[dir->fcb[temp].first-8]);
    int flag=-1;
    for(int i=2; i < FCBCOUNT; i++ ) {
        if (dir->fcb[i].free == 1){
            flag=1;
            break;
        }
    }
    //要删除的目录不为空
    if(flag==-1)
    {
        printf("rmdir: failed to remove '%s': Directory not empty\n",dirname);
        if(fd!=-1)
            close(fd);
        ptrcuridr=tempp;
        return 0;
    }
    //检查目录是否已打开，打开就关闭
    //回收该目录文件所占据的磁盘块，修改FAT
    disk->FAT1[dir->fcb[temp].first]=disk->FAT1[dir->fcb[temp].first]=0;
    //当前目录文件中清空该目录文件的目录项,memset清理干净
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