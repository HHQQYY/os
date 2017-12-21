#include "OS.h"
int open_path(char* dirname);
int dowrite(int fd,char *text,int len, char wstyle);
/*--------------全局变量-------------------------*/
extern char* myvhard;//虚拟磁盘起始地址
extern string currentdir;//当前目录
extern string cmd; //读取指令
extern USEROPEN openfilelist[MAXOPENFILE];//文件打开表
extern USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
extern DISK* disk;//将内容结构化
extern char command[16];//文件名标示符

/*---------------写文件函数----------------*/

int filewrite(int fd) {
    int way = 0;
    //检查fd的有效性
    if (fd >= MAXOPENFILE || fd <= 0) {
        printf("filewirt  ERROR:Is not a legitimate fd \n");
        return -1;
    }else if(openfilelist[fd].topenfile==0){
        printf("filewrite ERROR:该文件没有被打开\n");
        return -1;
    }
    while (1) {
        //提示等待用户输入写方式
        printf(" ------Please enter the way to write---------\n ");
        //1 : 截断写 2: 覆盖写 3: 追加写
        printf(" ------1:TRUNC 2:OVER 3:APPEND---------\n ");
        scanf("%d", &way);
        if (1 <= way && way <= 3) break;
        else printf("Input Error,Please Try Again\n");
    }
    // 如果是截断写,释放文件除第一块外的磁盘空间内容
    //内存用户打开表中文件长度为0,,读写指针置为0
    if (way == 1) {
        //释放文件除第一块外的磁盘空间内容
        int TEMP = 0;
        int ok = 1;
        for (int p = openfilelist[fd].fcb.first; p != -1; p = TEMP) {
            TEMP = disk->FAT1[p];
            if (ok != 1) {
                disk->FAT1[p] = disk->FAT2[p] = 0;
            }
            else {
                ok = 0;
            }
        }
        //长度置为0
        openfilelist[fd].fcb.length = 0;
        //读写指针置为0
        openfilelist[fd].count = 0;
    }
        //如果是追加写,修改文件的当前读写指针到文件的末尾
    else if (way == 3) {
        openfilelist[fd].count = openfilelist[fd].fcb.length;
    }
    //提示用户，输入内容通过CTRL+Z结束,用户可分多次输入写入内容,每次用回车结束
    printf(" Input CTRL+D end the input\n ");
    int temp=0;
    char buffer[3000];
    while(gets(buffer)!=0){
        int len=strlen(buffer);
        buffer[len]='\n';
        buffer[len+1]='\0';
        int ret=dowrite(fd,buffer,strlen(buffer),way);
        if(ret==-1) {
            return -1;
        }
        else temp+=ret;
    }
    //如果当前读写指针位置大于长度，则更新长度,并置fcbstate置1
    if(openfilelist[fd].count>openfilelist[fd].fcb.length) {
        openfilelist[fd].fcb.length = openfilelist[fd].count;
        openfilelist[fd].fcbstate=1;
    }
    //返回实际写入的字节
    return temp;
}

