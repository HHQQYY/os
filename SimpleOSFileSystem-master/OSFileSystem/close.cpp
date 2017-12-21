#include "OS.h"
int open_path(char* dirname);
/*--------------全局变量-------------------------*/
extern char* myvhard;//虚拟磁盘起始地址
extern string currentdir;//当前目录
extern string cmd; //读取指令
extern USEROPEN openfilelist[MAXOPENFILE];//文件打开表
extern USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
extern DISK* disk;//将内容结构化
extern char command[16];//文件名标示符

/*---------------关闭文件函数-----------------*/
int close(int fd){
    //检查fd的合法性
    if(fd>=MAXOPENFILE||fd<=0){
        printf(" Is not a legitimate fd \n");
        return -1;
    }

    //检查用户打开文件表表项的`fcbstate`字段,如果是1,则需要将该文件的FCB的内容保存的虚拟磁盘上该文件的目录项
    //方法是:打开该文件的父目录文件，已覆盖写方式调用do_wirte()将欲关闭的FCB写入父目录文件的相应盘块.
    if(openfilelist[fd].fcbstate==1&&openfilelist[fd].fcb.attribute==1){//处理目录的关闭
        char buffer[30]="..";
         int fatherfd=open_path(buffer);
        if(fatherfd!=-1) {
            //找到相应的盘号
            int pan1 = openfilelist[fatherfd].fcb.first;
            int area1 = -1;
            //盘号上相应的位置
            DirFile *dirson = (DirFile *) (disk->Data[pan1 - 8]);
            for (int i = 0; i < FCBCOUNT; i++) {
                if (dirson->fcb[i].free == 1  &&
                    strcmp(dirson->fcb[i].filename, openfilelist[fd].fcb.filename) == 0) {
                    //找到了该文件,覆盖fcb
                    dirson->fcb[i] = openfilelist[fd].fcb;
                    break;
                }
            }
            //关文件
            if(fatherfd!=0)
            close(fatherfd);
        }
    }else  if(openfilelist[fd].fcbstate==1&&openfilelist[fd].fcb.attribute==0){//处理目录的关闭
        char buffer[80];
        strcpy(buffer,openfilelist[fd].dir);
        int fatherfd=open_path(buffer);
        int pan1;
        if(fatherfd!=-1) {
            //找到相应的盘号
            pan1 = openfilelist[fatherfd].fcb.first;
            if(fatherfd!=0)
            close(fatherfd);
        }else{
            pan1 = ptrcuridr->fcb.first;
        }
            int area1 = -1;
            //盘号上相应的位置
            DirFile *dirson = (DirFile *) (disk->Data[pan1 - 8]);
            for (int i = 0; i < FCBCOUNT; i++) {
                if (dirson->fcb[i].free == 1  &&
                    strcmp(dirson->fcb[i].filename, openfilelist[fd].fcb.filename) == 0) {
                    //找到了该文件,覆盖fcb
                    dirson->fcb[i] = openfilelist[fd].fcb;
                    break;
                }
            }

        }
    //回收该文件占据的用户打开表表项(clear),topenfile字段置0
    memset(openfilelist+fd,0,sizeof(openfilelist[0]));
    openfilelist[fd].topenfile=0;
    //返回
    return 0;
}