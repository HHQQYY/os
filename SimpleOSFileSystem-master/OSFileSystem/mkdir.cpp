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




int mkdir(char *dirname)
{
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


    //-------------以下为一天前的代码-----------------------//
    //读取当前目录的地址
    int BlockDirNum=(ptrcuridr->fcb).first;
    DirFile *dir=(DirFile *)disk->Data[BlockDirNum-8];
    //遍历文件目录,检查是否有文件名相同的文件或目录,并找一个没有被使用的目录空闲表项
    int temp=-1, DirFreeItems =-1;
    for(int i=0;i<FCBCOUNT;i++) {
        if (dir->fcb[i].free == 1 && strcmp(dir->fcb[i].filename, dirname) == 0)//表项被使用,且是目录,且文件名相等
        {
            temp = i;//重名的表项
            break;
        }
        else if (dir->fcb[i].free == 0) {
            DirFreeItems = i;//目录空闲表项
        }
    }
    //如果文件名已存在,报错并退出
    if(temp!=-1)
    {
        printf("mkdir: cannot create directory '%s': Directory exists\n",dirname);
        if(fd!=-1)
            close(fd);
        ptrcuridr=tempp;
        return 0;
    }
    //如果没有空闲位置，报错退出
    if(DirFreeItems==-1)
    {
        printf("mkdir: cannot create directory '%s': Directory is full \n",dirname);
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
        printf("mkdir: cannot create directory '%s': Disk is full \n",dirname);
        if(fd!=-1)
            close(fd);
        ptrcuridr=tempp;
        return 0;
    }
    /*----------------开始新建目录-------------------*/
    //修改长度,fcbstate置为1
    ptrcuridr->fcbstate=1;
    ptrcuridr->fcb.length++;//不包括.和..的
    //分配FAT的空闲块,2表示被目录使用
    disk->FAT1[FATFreeItems]=disk->FAT2[FATFreeItems]=2;
    //将改块分配到 当前目录的空闲项目下
    strcpy(dir->fcb[DirFreeItems].filename,dirname);
    dir->fcb[DirFreeItems].free=1;//被使用
    dir->fcb[DirFreeItems].attribute=1;//是目录
    dir->fcb[DirFreeItems].first=FATFreeItems;//分配FAT空闲块
    dir->fcb[DirFreeItems].length=0;
    dir->fcb[DirFreeItems].data=0;//时间之后弄
    dir->fcb[DirFreeItems].time=0;//时间之后弄
    //进入下一次目录,初始化新获得的块(重置给予"."和"..")
    dir=(DirFile*)(disk->Data[FATFreeItems-8]);
    dir->init(BlockDirNum,FATFreeItems);
    /*----------------恢复现场------------------------*/
    if(fd!=-1)
      close(fd);
    ptrcuridr=tempp;
    return 1;
}