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

/*---------------实际写文件函数----------------*/
int dowrite(int fd,char *text,int len, char wstyle) {
    //申请1024字节的缓冲区buf
    char *buf = (char *) malloc(1024);
    if (buf == NULL) {
        printf("MALLOC ERROR b\n");
        return -1;
    }
    int tmplen = 0;
    int textpos = 0;
    int textlen = strlen(text);
    //将最终指针转换为逻辑块块号blockno 和 块内偏移blockoff;

    /*--------------------预先分配-----------------*/
    while(tmplen<len&& textpos < textlen) {
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
        int pre = currentblock;
        if (cnt != blockno + 1)//如果找不到这样的一块,那么还需要给它分配blockno+1-cnt块
        {
            //从currentblock开始分配
            for (int i = 1; i <= blockno + 1 - cnt; i++) {
                //检查FAT中是否有空闲的盘块
                int FATFreeItems = -1;
                for (int j = 0; j < BLOCKCOUNT; j++) {
                    if (disk->FAT1[j] == 0) {//没被使用的块标记为0
                        FATFreeItems = j;//找到了一个空闲块
                        break;
                    }
                }
                //如果FAT没有空闲块,报错退出
                if (FATFreeItems == -1) {
                    printf("FAT IS FULL\n");
                    return -1;
                }
                disk->FAT1[pre]=disk->FAT2[pre] = FATFreeItems;
                disk->FAT1[FATFreeItems]=disk->FAT2[FATFreeItems]=-1;
                pre = FATFreeItems;
            }
        }
        //如果是覆盖写，或者块内偏移off不等于0，则将blkno的虚拟磁盘块全部写入buff中，否则memset
        if (wstyle == 2 || blockoff != 0) {
            memcpy(buf, disk->Data[currentblock - 8], 1024);
        }
        else memset(buf,0,sizeof(buf));
        //将text中的内容暂存到缓冲区buff的第off字节开始的位置,直到缓冲区满
        for (int i = blockoff; i < 1024 && textpos < textlen && tmplen<len; i++) {
            buf[i] = text[textpos];
            textpos++;
            tmplen++;  //读入长度
        }
        memcpy(disk->Data[currentblock - 8], buf, 1024);
        openfilelist[fd].count += tmplen;
    }
    free(buf);
    return tmplen;
}
