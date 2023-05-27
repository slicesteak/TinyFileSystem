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

//设备驱动模块，直接负责磁盘文件直接读写
DeviceDriver g_DeviceDriver;

/* 高速缓存管理模块，通过管理缓存控制块来管理缓存,
* 包括申请、释放、读写、清空一块缓存的功能函数接口，以及系统退出时刷新所有缓存块*/
BufferManager g_BufferManager;

/* 打开文件管理模块，负责对打开文件的管理
* 为用户打开文件建立数据结构之间的勾连关系，为用户提供直接操作文件的文件描述符接口*/
OpenFileTable g_OpenFileTable; //
SuperBlock g_SuperBlock; //
FileSystem g_FileSystem; //系统盘块管理模块，主要负责对镜像文件的存储空间管理，包括SuperBlock 空间占用、DiskINode 空间分布、数据块区空间分布的管理。需要提供分配、回收 DiskINode 节点、数据块节点以及格式化磁盘文件的接口
INodeTable g_INodeTable;
FileManager g_FileManager;  //系统文件操作功能实现模块，主要封装文件系统中对文件处理的操作过程，负责对文件系统访问的具体细节。包括打开文件、创建文件、关闭文件、Seek 文件指针、读取文件、写入文件、删除文件等系统功能实现
User g_User;//用户操作接口模块，主要将用户的界面执行命令转化为对相应函数的调用，同时对输出进行处理，也包含检查用户输入的正确性与合法性
Operate g_Operate;//用户操作
int ffor_flag = 0; //初始化标志


int main() {
	g_Operate.display();
	return 0;
}
