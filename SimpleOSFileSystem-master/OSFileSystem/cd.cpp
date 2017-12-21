#include "OS.h"
int open(char *filename);
int close(int fd);
int open_path(char *dirname);
/*--------------全局变量-------------------------*/
extern char* myvhard;//虚拟磁盘起始地址
extern string currentdir;//当前目录
extern string cmd; //读取指令
extern USEROPEN openfilelist[MAXOPENFILE];//文件打开表
extern USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
extern DISK* disk;//将内容结构化
extern char command[50];//文件名标示符

/*----------------------更改当前目录函数---------------------*/
int cd(char *dirname) {
        USEROPEN *temp = ptrcuridr;//暂时保管一下ptrcuridr原始值,可能要回溯
        int fd=open_path(dirname);
        if (fd != -1) {
            //关掉旧的描述符
           int old=temp->pos;
            //获取旧的描述符
            if (old != 0)   //根目录描述符不关
                close(old);
            //工作目录指向它
            ptrcuridr = openfilelist + fd;
            //当前目录赋值
            currentdir= ptrcuridr->dir ;
            currentdir+= '/';
            currentdir+= ptrcuridr->fcb.filename;
            return 0;
        }
        else {
            ptrcuridr = temp;
            printf("No such file or directory\n");
            return 0;
        }
    }
