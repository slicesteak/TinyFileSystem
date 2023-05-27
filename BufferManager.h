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
    static const int NBUF = 100;			/* ������ƿ顢������������ */
    static const int BUFFER_SIZE = 512;		/* ��������С�� ���ֽ�Ϊ��λ */

private:
    Buffer* bufferList;							/* ������п��ƿ� */
    Buffer nBuffer[NBUF];						/* ������ƿ����� */
    unsigned char buffer[NBUF][BUFFER_SIZE];	/* ���������� */
    unordered_map<int, Buffer*> map;
    DeviceDriver* deviceDriver;  //�����д�����ļ�

public:
    BufferManager();
    ~BufferManager();

	/* ����һ�黺�棬���ڶ�д�豸�ϵĿ�blkno��*/
    Buffer* GetBlk(int blkno);//�Ὣԭ��������д����ٷ����������̿�
    
	/* �ͷŻ�����ƿ�buf */
	void Brelse(Buffer* bp);//�����ǽ�������ƿ���뵽����β��,��Ϊ���ӳ�д,���ǵ�ʱ��Ż�д�����)

	/* ��һ�����̿飬blknoΪĿ����̿��߼���š� */
    Buffer* Bread(int blkno);//

	/* дһ�����̿� */
    void Bwrite(Buffer* bp);//д֮ǰ��Ҫ��ԭ����д�����
    
	/* �ӳ�д���̿� */
	void Bdwrite(Buffer* bp);//�ı仺����ƿ�flag,Ȼ������뵽β��

	/* ��ջ��������� */
    void Bclear(Buffer* bp);//����ԭ��������,������д��
    
	/* ���������ӳ�д�Ļ���ȫ����������� */
	void Bflush();
    
    /* ��ȡ���п��ƿ�Buf�������� */
	//Buffer& GetFreeBuffer();				
	
    /* ��ʽ������Buffer */
    void FormatBuffer();//��ʼ�� һ�� �ֲ�����buffer,Ȼ��������ȫ����Ա��ֵ

private:
    void InitList();
    void DetachNode(Buffer* pb); //���ı�ѭ�������ϵ,ȡһ����㵽β��
    void InsertTail(Buffer* pb);//���ı�ѭ�������ϵ,����һ����㵽β��
    void debug(); //debugר��, ���ѭ���������л�����ƿ������
};

#endif