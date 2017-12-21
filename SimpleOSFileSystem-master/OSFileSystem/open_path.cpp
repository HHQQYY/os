#include "OS.h"
int open(char *filename);
int close(int fd);
/*--------------全局变量-------------------------*/
extern char* myvhard;//虚拟磁盘起始地址
extern string currentdir;//当前目录
extern string cmd; //读取指令
extern USEROPEN openfilelist[MAXOPENFILE];//文件打开表
extern USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
extern DISK* disk;//将内容结构化
extern char command[50];//文件名标示符

/*----------------------更改当前目录函数---------------------

读入一条文件路径,返回一个打开的fd

 支持绝对路径和相对路径.

 输出，如果路径正确，返回文件打开后的fd.

------------------------------------------------------*/
int open_path(char *dirname) {
    //如果dirname是绝对路径
    int fd=0, ok = 1, fdtemp = -1;//ok代表是否找到dirname所指的文件,fdtemp存临时的,用来关闭
    USEROPEN *temp = ptrcuridr;//暂时保管一下ptrcuridr原始值,可能要回溯

    if (dirname[0] == '/') {
        ptrcuridr = openfilelist;//我们的openfilelist[0]永远是根目录
        //使工作目录暂时指向根目录
        char *p = strtok(dirname + 1, "/");//用“/”分割 dirname[1]开始的字符串
        if (p != NULL) p = strtok(NULL, "/");//跳过/home
        while (p) {
            fd = open(p);
            if (fd == -1) {
                ok = 0;
                if (fdtemp != -1)  //离开前记得关文件
                    close(fdtemp);//把上个打开的文件关掉
                break;
            }
            ptrcuridr = openfilelist + fd;
            if (fdtemp != -1)
                close(fdtemp);//把上个打开的文件关掉
            fdtemp = fd;
            p = strtok(NULL, "/");
        }
    }
        //如果是相对路径
    else {
        int len=strlen(dirname);
        dirname[len]='/';
        dirname[len+1]=0;
        char *p = strtok(dirname, "/");//用“/”分割 dirname[1]开始的字符串
        while (p) {
            fd = open(p);
            if (fd == -1) {
                ok = 0;
                if (fdtemp != -1)  //离开前记得关文件
                    close(fdtemp);//把上个打开的文件关掉
                break;
            }
            ptrcuridr = openfilelist + fd;
            if (fdtemp != -1)
                close(fdtemp);//把上个打开的文件关掉
            fdtemp = fd;
            p = strtok(NULL, "/");
        }
    }
         ptrcuridr = temp;
        //输出数据
        if (ok == 1)
            return fd;
        else
            return -1;

}
