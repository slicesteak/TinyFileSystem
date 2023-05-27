#ifndef  USER_H
#define USER_H

#include "FileManager.h"
#include <string>
using namespace std;


class User {
public:
	static const int EAX = 0;	/* u.ar0[EAX]�������ֳ���������EAX�Ĵ�����ƫ���� */
	static const int UNAME = 6;/*�û�������*/
	static const int UPASSWD = 10;/*���볤��*/

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
		U_NOTFILE = 99,     /* �򿪵Ĳ���flie���� */
	};

public:
	INode* cdir;		/* ָ��ǰĿ¼��Inodeָ�� */
	INode* pdir;		/* ָ��Ŀ¼��Inodeָ�� */

	DirectoryEntry dent;					/* ��ǰĿ¼��Ŀ¼�� */
	char dbuf[DirectoryEntry::DIRSIZ];	    /* ��ǰ·������ */
	string curDirPath;						/* ��ǰ����Ŀ¼����·�� */

	string dirp;				/* ϵͳ���ò���(һ������Pathname)��ָ�� */
	long arg[5];				/* ��ŵ�ǰϵͳ���ò��� */
								/* ϵͳ������س�Ա */
	unsigned int	ar0[5];	    /* ָ�����ջ�ֳ���������EAX�Ĵ���
									��ŵ�ջ��Ԫ�����ֶδ�Ÿ�ջ��Ԫ�ĵ�ַ��
									��V6��r0���ϵͳ���õķ���ֵ���û�����
									x86ƽ̨��ʹ��EAX��ŷ���ֵ�����u.ar0[R0] */
	ErrorCode u_error;			/* ��Ŵ����� */

	OpenFiles ofiles;		    /* ���̴��ļ������������ */

	IOParameter IOParam;	    /* ��¼��ǰ����д�ļ���ƫ�������û�Ŀ�������ʣ���ֽ������� */

    FileManager* fileManager;  //�ṩread() write() open()�Ⱥ������û�ʹ��

	string ls;


	char pw_name[UNAME];        /*�û���������*/
	char pw_password[UPASSWD];
	int pw_uid;   /*��ֵ�û�id*/
	int pw_gid; /*��ֵ��id*/
	string pw_dir;	/*��ʼ����Ŀ¼*/


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