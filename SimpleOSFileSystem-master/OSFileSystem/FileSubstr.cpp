//
// 切割字符串，例如/A/B/C/D 切割成 /A/B/C 和 D
//
#include "OS.h"

/*--------------全局变量-------------------------*/
extern char* myvhard;//虚拟磁盘起始地址
extern string currentdir;//当前目录
extern string cmd; //读取指令
extern USEROPEN openfilelist[MAXOPENFILE];//文件打开表
extern USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
extern DISK* disk;//将内容结构化
extern char command[50];//文件名标示符
/*-----------------------------------------------*/

int FileSubstr(char *str){
    int len=strlen(str);
    int cnt=0,flag=0;
    for(int i=1;i<len-1;i++)
    {
        if(str[i]=='/')
        {
            cnt++;
            flag=i;
        }
    }
    if(cnt==0) return -1;
    else return flag;
}