#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "Buffer.h"
#include "DeviceDriver.h"
#include"stdc++.h"
#include<iostream>
#include <unordered_map>
using namespace std;

class BufferManager {
public:
    static const int NBUF = 100;			/* 缓存控制块、缓冲区的数量 */
    static const int BUFFER_SIZE = 512;		/* 缓冲区大小。 以字节为单位 */

private:
    Buffer* bufferList;							/* 缓存队列控制块 */
    Buffer nBuffer[NBUF];						/* 缓存控制块数组 */
    unsigned char buffer[NBUF][BUFFER_SIZE];	/* 缓冲区数组 */
    unordered_map<int, Buffer*> map;
    DeviceDriver* deviceDriver;  //负责读写镜像文件

public:
    BufferManager();
    ~BufferManager();

	/* 申请一块缓存，用于读写设备上的块blkno。*/
    Buffer* GetBlk(int blkno);//会将原来的内容写入后再分配给这个磁盘块
    
	/* 释放缓存控制块buf */
	void Brelse(Buffer* bp);//仅仅是将缓存控制块插入到链表尾部,因为是延迟写,覆盖的时候才会写入磁盘)

	/* 读一个磁盘块，blkno为目标磁盘块逻辑块号。 */
    Buffer* Bread(int blkno);//

	/* 写一个磁盘块 */
    void Bwrite(Buffer* bp);//写之前需要将原内容写入磁盘
    
	/* 延迟写磁盘块 */
	void Bdwrite(Buffer* bp);//改变缓存控制块flag,然后将其插入到尾部

	/* 清空缓冲区内容 */
    void Bclear(Buffer* bp);//覆盖原来的内容,并不会写回
    
	/* 将队列中延迟写的缓存全部输出到磁盘 */
	void Bflush();
    
    /* 获取空闲控制块Buf对象引用 */
	//Buffer& GetFreeBuffer();				
	
    /* 格式化所有Buffer */
    void FormatBuffer();//初始化 一个 局部变量buffer,然后让它给全部成员赋值

private:
    void InitList();
    void DetachNode(Buffer* pb); //仅改变循环链表关系,取一个结点到尾部
    void InsertTail(Buffer* pb);//仅改变循环链表关系,插入一个结点到尾部
    void debug(); //debug专用, 输出循环链表所有缓存控制块的内容
};

#endif