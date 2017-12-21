#include "OS.h"
/*--------------全局变量-------------------------*/
extern char* myvhard;//虚拟磁盘起始地址
extern string currentdir;//当前目录
extern string cmd; //读取指令
extern USEROPEN openfilelist[MAXOPENFILE];//文件打开表
extern USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
extern DISK* disk;//将内容结构化
extern char command[50];//文件名标示符
/*--------------------------------磁盘格式化函数-------------------*/
int format() {
    memset(myvhard,0,DISKSIZE);
    //创建根目录,在磁盘块8

    //前九个被FAT1+FAT2+root占用
    for(int i=0;i<9;i++){
        disk->FAT1[i]=disk->FAT2[i]=-2;//-2代表被占用
    }
    DirFile *dir=(DirFile *)disk->Data[8-8];//注意Data和FAT的区别
    //初始化根目录.
    dir->init(8,8);
    return 1;
}


/*--------------------------------进入文件系统函数--------------------*/
int startsys() {
    myvhard=(char *)malloc(DISKSIZE);   //申请1024*1000磁盘空间
    disk=(DISK *)myvhard;
    FILE *fp=fopen("myfsys","r");
    if(fp!=NULL)
    {
        printf("|-------------------------------------------|\n");
        printf("|-----------myfsys is loading---------------|\n");
        printf("|-------------------------------------------|\n\n");
        fread(myvhard,sizeof(char),DISKSIZE,fp);
        fclose(fp);
    }
    else{
        printf("|-------------------------------------------|\n");
        printf("|-----------myfsys is not exit--------------|\n");
        printf("|--File system is being created now --------|\n");
        printf("|-------------------------------------------|\n\n");
        format();
    }
    //初始化用户打开表
    memset(openfilelist,0,sizeof(openfilelist));
    //将根目录打开,首先修改fcb里的内容
    openfilelist->fcb.first=8;
    //文件打开表项的内容
    openfilelist[0].topenfile=1;
    strcpy(openfilelist->dir,"");
    strcpy(openfilelist->fcb.filename,"home");
    strcpy(openfilelist->fatherfilename,"");
    ptrcuridr=&openfilelist[0];
    openfilelist[0].pos=0;
    //当前目录设置为根目录
    currentdir=Path;
    return 1;
}
