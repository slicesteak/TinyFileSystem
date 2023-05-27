#ifndef  USER_H
#define USER_H

#include "FileManager.h"
#include <string>
using namespace std;


class User {
public:
	static const int EAX = 0;	/* u.ar0[EAX]；访问现场保护区中EAX寄存器的偏移量 */
	static const int UNAME = 6;/*用户名长度*/
	static const int UPASSWD = 10;/*密码长度*/

	enum ErrorCode {
		U_NOERROR = 0,  	/* No u_error */
		U_ENOENT = 2,	    /* No such file or directory */
		U_EBADF = 9,	    /* Bad file number */
		U_EACCES = 13,	    /* Permission denied */
		U_EEXIST = 17,	/* File exists */
		U_ENOTDIR = 20,	    /* Not a directory */
		U_ENFILE = 23,	    /* File table overflow */
		U_EMFILE = 24,	    /* Too many open files */
		U_EFBIG = 27,	    /* File too large */
		U_ENOSPC = 28,	    /* No space left on device */
		U_NOTFILE = 99,     /* 打开的不是flie类型 */
	};

public:
	INode* cdir;		/* 指向当前目录的Inode指针 */
	INode* pdir;		/* 指向父目录的Inode指针 */

	DirectoryEntry dent;					/* 当前目录的目录项 */
	char dbuf[DirectoryEntry::DIRSIZ];	    /* 当前路径分量 */
	string curDirPath;						/* 当前工作目录完整路径 */

	string dirp;				/* 系统调用参数(一般用于Pathname)的指针 */
	long arg[5];				/* 存放当前系统调用参数 */
								/* 系统调用相关成员 */
	unsigned int	ar0[5];	    /* 指向核心栈现场保护区中EAX寄存器
									存放的栈单元，本字段存放该栈单元的地址。
									在V6中r0存放系统调用的返回值给用户程序，
									x86平台上使用EAX存放返回值，替代u.ar0[R0] */
	ErrorCode u_error;			/* 存放错误码 */

	OpenFiles ofiles;		    /* 进程打开文件描述符表对象 */

	IOParameter IOParam;	    /* 记录当前读、写文件的偏移量，用户目标区域和剩余字节数参数 */

    FileManager* fileManager;  //提供read() write() open()等函数给用户使用

	string ls;


	char pw_name[UNAME];        /*用户名和密码*/
	char pw_password[UPASSWD];
	int pw_uid;   /*数值用户id*/
	int pw_gid; /*数值组id*/
	string pw_dir;	/*初始工作目录*/


public:
	User();
	~User();

	void Login();
	void Logout();
	void Ls();
	void Cd(string dirName);
	void Mkdir(string dirName);
	void Create(string fileName, string mode);
	void Delete(string fileName);
	bool Open(string fileName, string mode);
	void Close(string fd);
	void Seek(string fd, string offset, string origin);
    void WritefromFile(string fd, string inFile, string size);
    void WritefromConsole(string fd, string content);
    char* Read(string fd, string outFile, string size);
	bool CheckPerm(INode path, static const unsigned int mode, int ffor_flag);
	void Copy(string srcFileName,string destFileName);
	void Move(string srcFileName, string destFileName);
	void init();
	void Pwd();
	void Link(string srcFileName, string destFileName);

private:
	bool IsError();
	void EchoError(enum ErrorCode err);
	int INodeMode(string mode);
	int FileMode(string mode);
	bool checkPathName(string path);
	void SetInfo(User temp, int uid);
};

#endif