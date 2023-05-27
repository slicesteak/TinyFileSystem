#ifndef BUFFER_H
#define BUFFER_H

#include<iostream>
#include<vector>
using namespace std;

//������ƿ�, ͨ��һ��������ƿ���һ��512�ֽڵ��ڴ�ĸ�����Ϣ
class Buffer {
public:
	
    /* flags�б�־λ */
	enum BufferFlag {
		B_DONE = 0x4,		    /* I/O�������� */
		B_DELWRI = 0x80		    /* �ӳ�д������Ӧ����Ҫ��������ʱ���ٽ�������д����Ӧ���豸�� */
	};

public:
	unsigned int flags;	    /* ������ƿ��־λ */

	//������ƿ����˫��ѭ������
	Buffer*	forw;   //ָ��ǰһ��������ƿ�
	Buffer*	back;  //ָ���һ��

	int		wcount;		    /* �贫�͵��ֽ��� */
	unsigned char* addr;	/* ָ��û�����ƿ�������Ļ��������׵�ַ */
	int		blkno;		    /* �����߼���� */
	int		u_error;	    /* I/O����ʱ��Ϣ */
	int		resid;		    /* I/O����ʱ��δ���͵�ʣ���ֽ��� */
    int no;
public:
	Buffer();
	~Buffer();

    void debugMark();   //debugר��,���buffer�е�����
    void debugContent();  //debugר��,��ʾ������е�����
};

#endif
