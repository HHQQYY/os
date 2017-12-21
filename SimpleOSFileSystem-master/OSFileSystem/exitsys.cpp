#include "OS.h"
int open_path(char* dirname);
int close(int fd);
/*--------------全局变量-------------------------*/
extern char* myvhard;//虚拟磁盘起始地址
extern string currentdir;//当前目录
extern string cmd; //读取指令
extern USEROPEN openfilelist[MAXOPENFILE];//文件打开表
extern USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
extern DISK* disk;//将内容结构化
extern char command[16];//文件名标示符

/*--------------退出文件系统函数------------------*/
void exitsys(){
    FILE * fd=fopen("myfsys","w");
    //关闭所有打开的文件
    for(int i=1;i<MAXOPENFILE;i++){
        if(openfilelist[i].topenfile==1)
            close(i);
    }
    fwrite(myvhard,sizeof(char),DISKSIZE,fd);
    fclose(fd);
    free(myvhard);
    exit(0);
}