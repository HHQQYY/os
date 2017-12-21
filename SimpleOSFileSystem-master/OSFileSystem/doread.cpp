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
/*------------------实际读文件函数--------------------------*/
//text的指向那个读出数据的用户地址
int doread(int fd,int len,char *text){
    //申请1024字节的缓冲区buf
    char *buf = (char *) malloc(1024);
    if (buf == NULL) {
        printf("MALLOC ERROR b\n");
        return -1;
    }
    int tmplen = 0;
    int textpos = 0;
    //将最终指针转换为逻辑块块号blockno 和 块内偏移blockoff;
    while(tmplen<len&&openfilelist[fd].count<openfilelist[fd].fcb.length) {
        int blockno = (openfilelist[fd].count) / 1024;
        int blockoff = (openfilelist[fd].count) % 1024;
        //寻找磁盘块blockno
        int currentblock = 0;
        int cnt = 0;
        for (int p = openfilelist[fd].fcb.first; p != -1; p = disk->FAT1[p]) {
            cnt++;
            currentblock = p;
            if (cnt == blockno + 1)
                break;
        }

          memcpy(buf, disk->Data[currentblock - 8], 1024);

        //
        for (int i = blockoff; i < 1024 && tmplen<len && openfilelist[fd].count<openfilelist[fd].fcb.length; i++) {
            text[textpos] = buf[i];
            textpos++;
            tmplen++;  //读入长度
            openfilelist[fd].count++;
        }
    }
        free(buf);
        return tmplen;
}