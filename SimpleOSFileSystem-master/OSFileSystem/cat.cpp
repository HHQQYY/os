#include "OS.h"
int open_path(char* dirname);
int dowrite(int fd,char *text,int len, char wstyle);
int doread(int fd,int len,char *text);
int fileread(int fd,int len);
int close(int fd);
int open(char *filename);
/*--------------全局变量-------------------------*/
extern char* myvhard;//虚拟磁盘起始地址
extern string currentdir;//当前目录
extern string cmd; //读取指令
extern USEROPEN openfilelist[MAXOPENFILE];//文件打开表
extern USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
extern DISK* disk;//将内容结构化
extern char command[16];//文件名标示符

int cat(char *filename){
    int fd=open(filename);
    if(fd==-1){
       printf("ERROR:\n");
        return -1;
    }
    fileread(fd,openfilelist[fd].fcb.length);
    close(fd);
}