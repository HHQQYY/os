//
// Created by Administrator on 2016-06-03.
//
#ifndef A_H_
#define A_H_
#include <cstdio>
#include <memory.h>
#include <string>
#include <iostream>
#include <malloc.h>
#include <time.h>
using namespace std;
/*常量定义*/
#define  Path  "/home" //根目录
#define BLOCKSIZE  1024 //磁盘块大小
#define BLOCKCOUNT  1000 //盘块大小
#define MAXOPENFILE 10       //能打开最多的文件数
#define DISKSIZE  (BLOCKSIZE*BLOCKCOUNT)//磁盘大小
#define END -1

/*-------------文件控制块 FCB--------------------*/
//定义的比较随性
struct FCB
{
    char filename[12]; //文件名
    char attribute;//0表示目录,1表示数据文件
    int time;//创建时间
    int data;//创建日期
    int first;//起始盘号
    int length;//长度
    char free;//表示目录项是否为空
};
const int FCBCOUNT = BLOCKSIZE/sizeof(FCB);//一个块的最多FCB数量

/*-----------------目录文件---------------------*/
struct DirFile{
    FCB fcb[FCBCOUNT];  //文件控制块
    void init(int father,int self)
    {
        //给根目录创建.. 和 .  序号0放".", 序号1放".."
        memset(fcb,0,sizeof(fcb));
        fcb[1].free=fcb[0].free=1;
        fcb[1].attribute=fcb[0].attribute=1;
        fcb[1].first=father;
        fcb[0].first=self;
        memcpy(fcb[0].filename,".",sizeof("."));
        memcpy(fcb[1].filename,"..",sizeof(".."));

    }
};

/*------------------磁盘------------------------*/
struct DISK
{
    int FAT1[BLOCKSIZE];//磁盘块0-3代表FAT,
    int FAT2[BLOCKSIZE];//磁盘块4-7代表FAT2
    char Data[BLOCKCOUNT-8][BLOCKSIZE];//目录和其他文件 磁盘块8~1000
};
/*----------------用户打开表---------------------*/
struct USEROPEN
{
    FCB fcb;
    char dir[80];//相应打开文件所在的目录名
    int count;//读写指针在文件的位置
    char fcbstate;//是否修改了文件的FCB内容，修改了置为1,否则置为0
    char topenfile;//表示该用户表项是否被占用，1就是被占用,0就是没有被占用
    char fatherfilename[12];//上一层目录的名字
    int pos;
};


#endif