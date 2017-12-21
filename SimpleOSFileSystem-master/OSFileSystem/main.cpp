#include "OS.h"
using namespace std;

/*-------------函数声明------------------------*/
void help();

int cd(char *dirname);

int startsys();

int format();

int mkdir(char *dirname);

int rmdir(char *dirname);

int close(int fd);

int open(char *filename);

int creat(char *filename);

int rm(char *filename);

int filewrite(int fd);

int dowrite(int fd,char *text,int len, char wstyle);

int fileread(int fd,int len);

int doread(int fd,int len,char *text);

void exitsys();

int cat (char *filename);
/*--------------全局变量-------------------------*/
char* myvhard;//虚拟磁盘起始地址
string currentdir="/home";//当前目录
string cmd; //读取指令
USEROPEN openfilelist[MAXOPENFILE];//文件打开表
USEROPEN *ptrcuridr;//当前目录在文件打开表的位置
DISK* disk;//将内容结构化
char command[50];//文件名标示符




/*--------------------- 显示目录函数 ---------------*/
void ls() {
    int BlockDirNum = (ptrcuridr->fcb).first;
    DirFile *dir = (DirFile *) disk->Data[BlockDirNum - 8];
    for (int i = 0; i < FCBCOUNT; i++) {
        if (dir->fcb[i].free == 1) {
            if (dir->fcb[i].attribute == 0)
                printf("%10s---length:%5d----File\n",dir->fcb[i].filename,dir->fcb[i].length);
            else
                printf("%10s---length:%5d----Directory\n",dir->fcb[i].filename,dir->fcb[i].length);
        }
    }
}



int main() {
    printf("Welcome the OS FileSystem\n");
    printf("input 'help' get more information\n\n\n");
   // freopen("E:\\OSFileSystem\\a.in","r",stdin);
    startsys(); //Init the System
    int len;
    while(1)
    {
        cout<<currentdir+">";
        cin>>cmd;
        if(cmd=="help"){            //帮助
            help();
        }
        else if(cmd=="mkdir"){
            cin>>command;
            mkdir(command);
        }
        else if(cmd=="cd"){
            cin>>command;
            cd(command);
        }
        else if(cmd=="exit") {
            break;
        }
        else if(cmd=="rmdir"){
            cin>>command;
            rmdir(command);
        }
        else if(cmd=="ls"){
            ls();
        }
        else if(cmd=="open"){
            cin>>command;
            open(command);
        }
        else if(cmd=="close"){
            cin>>command;
            close(atoi(command));
        }
        else if(cmd=="creat"){
            cin>>command;
            creat(command);
        }
        else if(cmd=="rm"){
            cin>>command;
            rm(command);
        }
            //
        else if(cmd=="write"){
            cin>>command;
            filewrite(atoi(command));
        } else if(cmd=="cat"){
            cin>>command;
            cat(command);
        }
        else if(cmd=="read") {
            cin >> command >> len;
            fileread(atoi(command),len);
        }
        else if(cmd=="exitsys"){
            exitsys();
        }else {
            printf("The cmd is not exits\n");
        }
    }
}