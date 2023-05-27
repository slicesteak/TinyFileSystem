#ifndef BUFFER_H
#define BUFFER_H

#include<iostream>
#include<vector>
using namespace std;

//缓存控制块, 通过一个缓存控制块标记一个512字节的内存的各项信息
class Buffer {
public:
	
    /* flags中标志位 */
	enum BufferFlag {
		B_DONE = 0x4,		    /* I/O操作结束 */
		B_DELWRI = 0x80		    /* 延迟写，在相应缓存要移做他用时，再将其内容写到相应块设备上 */
	};

public:
	unsigned int flags;	    /* 缓存控制块标志位 */

	//缓存控制块采用双向循环链表
	Buffer*	forw;   //指向前一个缓存控制块
	Buffer*	back;  //指向后一个

	int		wcount;		    /* 需传送的字节数 */
	unsigned char* addr;	/* 指向该缓存控制块所管理的缓冲区的首地址 */
	int		blkno;		    /* 磁盘逻辑块号 */
	int		u_error;	    /* I/O出错时信息 */
	int		resid;		    /* I/O出错时尚未传送的剩余字节数 */
    int no;
public:
	Buffer();
	~Buffer();

    void debugMark();   //debug专用,输出buffer中的内容
    void debugContent();  //debug专用,显示缓存块中的内容
};

#endif
