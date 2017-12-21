#include "OS.h"
int open_path(char* dirname);
int dowrite(int fd,char *text,int len, char wstyle);
int doread(int fd,int len,char *text);
/*--------------全局变量-------------------------*/
extern char* myvhard;//虚拟磁盘起始地址
extern string currentdir;//当前目录
extern string cmd; //读取指令
extern USEROPEN openfilelist[MAXOPENFILE];//文件打开表
extern USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
extern DISK* disk;//将内容结构化
extern char command[16];//文件名标示符

/*---------------读文件函数----------------*/
const  int MAXSIZE=1024*50;
int fileread(int fd,int len){
    char text[MAXSIZE];
    memset(text,0,sizeof(text));
    //检查fd的有效性
    if (fd >= MAXOPENFILE || fd <= 0) {
        printf("filewirt:Is not a legitimate fd \n");
        return -1;
    }else if(openfilelist[fd].topenfile==0){
        printf("filewrite ERROR:The File Don't Open\n");
        return -1;
    }
    //调用do_read()读取指定文件的len字节内容到text[]中.
    int rt=doread(fd,len,text);
    //如果do_read()返回值为负,则显示出错信息,否则将text[]中的内容显示到屏幕上;
    if(rt==-1){
        printf("READ FAIL");
        return -1;
    }else{
        //输出text的内容
         for(int i=0;i<len;i++){
             printf("%c",text[i]);
         }
          printf("\n");
    }
}