#include "INode.h"
#include "File.h"
#include "FileSystem.h"
#include "OpenFileManager.h"
#include "FileManager.h"
#include "User.h"
#include "Operate.h"
#include <iostream>
#include <unordered_map>


using namespace std;

//�豸����ģ�飬ֱ�Ӹ�������ļ�ֱ�Ӷ�д
DeviceDriver g_DeviceDriver;

/* ���ٻ������ģ�飬ͨ����������ƿ���������,
* �������롢�ͷš���д�����һ�黺��Ĺ��ܺ����ӿڣ��Լ�ϵͳ�˳�ʱˢ�����л����*/
BufferManager g_BufferManager;

/* ���ļ�����ģ�飬����Դ��ļ��Ĺ���
* Ϊ�û����ļ��������ݽṹ֮��Ĺ�����ϵ��Ϊ�û��ṩֱ�Ӳ����ļ����ļ��������ӿ�*/
OpenFileTable g_OpenFileTable; //
SuperBlock g_SuperBlock; //
FileSystem g_FileSystem; //ϵͳ�̿����ģ�飬��Ҫ����Ծ����ļ��Ĵ洢�ռ��������SuperBlock �ռ�ռ�á�DiskINode �ռ�ֲ������ݿ����ռ�ֲ��Ĺ�����Ҫ�ṩ���䡢���� DiskINode �ڵ㡢���ݿ�ڵ��Լ���ʽ�������ļ��Ľӿ�
INodeTable g_INodeTable;
FileManager g_FileManager;  //ϵͳ�ļ���������ʵ��ģ�飬��Ҫ��װ�ļ�ϵͳ�ж��ļ�����Ĳ������̣�������ļ�ϵͳ���ʵľ���ϸ�ڡ��������ļ��������ļ����ر��ļ���Seek �ļ�ָ�롢��ȡ�ļ���д���ļ���ɾ���ļ���ϵͳ����ʵ��
User g_User;//�û������ӿ�ģ�飬��Ҫ���û��Ľ���ִ������ת��Ϊ����Ӧ�����ĵ��ã�ͬʱ��������д���Ҳ��������û��������ȷ����Ϸ���
Operate g_Operate;//�û�����
int ffor_flag = 0; //��ʼ����־


int main() {
	g_Operate.display();
	return 0;
}
