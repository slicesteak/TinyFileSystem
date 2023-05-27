#include "User.h"
#include "Utility.h"
#include "Operate.h"
#include<conio.h>
#include<Windows.h>
#include <iostream>
#include <fstream>
using namespace std;

extern FileManager g_FileManager;
extern DeviceDriver g_DeviceDriver;
extern Operate g_Operate;
extern int ffor_flag;
extern User g_User;
User::User() {
	u_error = U_NOERROR;
	fileManager = &g_FileManager;
	dirp = "/";
	curDirPath = "/";
	cdir = fileManager->rootDirINode;
	pdir = NULL;
	pw_uid = -1;
	pw_gid = -1;
	Utility::memset(arg, 0, sizeof(arg));
}

User::~User() {
}

void User::init() {
	g_User.pw_uid = 0;
	g_User.pw_gid = 0;
	Mkdir("/home");
	Mkdir("/etc");
	Create("/etc/password", "-rw");
	Open("/etc/password", "-rw");
	int temp = ar0[EAX];
	WritefromFile(to_string(temp), "pass.txt", "1992");
	Close(to_string(temp));
	g_User.pw_uid = 1;
	g_User.pw_gid = 1;
	Mkdir("/home/user1");
	g_User.pw_uid = 2;
	g_User.pw_gid = 1;
	Mkdir("/home/user2");
}

/*检查是否拥有当前所需权限*/
bool User::CheckPerm(INode path, static const unsigned int mode, int ffor_flag) {
	if (pw_uid == 0) //如果是管理员,则直接返回true
	{
		return true;
	}

	if (ffor_flag) {
		return true;
	}
	int temp_mode = mode;
	if (pw_uid == path.i_uid) {
		if ((temp_mode & INode::IRWXU) == temp_mode) {
			return true;
		}
	}
	else if (pw_gid == path.i_gid) {
		temp_mode = mode >> 3;
		if ((temp_mode & INode::IRWXG) == temp_mode) {
			return true;
		}
	}
	else {
		temp_mode = mode >> 6;
		if ((temp_mode & INode::IRWXO) == temp_mode) {
			return true;
		}
	}
	u_error = U_EACCES;
	return false;
}

void User::SetInfo(User temp, int uid) {
	u_error = U_NOERROR;
	dirp = "/";
	curDirPath = temp.pw_dir;
	pw_uid = temp.pw_uid;
	pw_gid = temp.pw_gid;
	//fileManager->Open();
	switch (uid)
	{
	case 0: {
		cdir = fileManager->rootDirINode;
		pdir = NULL;
		break;
	}
	case 1: {
		cdir = fileManager->user1DirINode;
		pdir = fileManager->homeDirINode;
		break;
	}
	case 2: {
		cdir = fileManager->user2DirINode;
		pdir = fileManager->homeDirINode;
		break;
	}
	default:
		break;
	}
	Utility::memset(arg, 0, sizeof(arg));
}

void User::Login() {
	char userName[UNAME];
	char passWord[UPASSWD];
	char ch;
	int i;
	Open("etc/password", "-r");
	string fp = to_string(ar0[EAX]);
	char* buffer = Read(fp, "", "1992");
	Close(fp);
	system("cls");
	char buf_root[664], buf_user1[664], buf_user2[664];
	for (int i = 0; i < 664; i++) {
		buf_root[i] = buffer[i];
	}
	for (int i = 664; i < 664 * 2; i++)
	{
		buf_user1[i - 664] = buffer[i];
	}
	for (int i = 664 * 2; i < 664 * 3; i++)
	{
		buf_user2[i - 664 * 2] = buffer[i];
	}
	int root_i = (int)buf_root;
	int user1_i = (int)buf_user1;
	int user2_i = (int)buf_user2;

	User* temp_root = (User*)root_i;
	User* temp_user1 = (User*)user1_i;
	User* temp_user2 = (User*)user2_i;

	while (1) {
		i = 0;
		cout << "User:";
		cin >> userName;
		cin.ignore();
		cout << "Password:";
		while (i < UPASSWD)
		{
			ch = getch();
			if (ch == '\r')
			{  //回车结束输入
				printf("\n");
				break;
			}
			if (ch == '\b' && i > 0)
			{  //按下删除键
				i--;
				printf("\b \b");
			}
			else if (isprint(ch))
			{  //输入可打印字符
				passWord[i] = ch;
				printf("*");
				i++;
			}
		}
		passWord[i] = 0;
		if ((!strcmp(userName, temp_root->pw_name)) && (!strcmp(passWord, temp_root->pw_password))) {
			SetInfo(*temp_root, 0);
			break;
		}
		else if ((!strcmp(userName, temp_user1->pw_name)) && (!strcmp(passWord, temp_user1->pw_password))) {
			SetInfo(*temp_user1, 1);
			break;
		}
		else if ((!strcmp(userName, temp_user2->pw_name)) && (!strcmp(passWord, temp_user2->pw_password))) {
			SetInfo(*temp_user2, 2);
			break;
		}
		else {
			cout << "Login failed" << endl;
		}
	}
}

void User::Logout() {
	u_error = U_NOERROR;
	fileManager = &g_FileManager;
	dirp = "/";
	curDirPath = "/";
	cdir = fileManager->rootDirINode;
	pdir = NULL;
	pw_uid = -1;
	pw_gid = -1;
	Utility::memset(arg, 0, sizeof(arg));
	cout << "User logout success!" << endl;
}

void User::Mkdir(string dirName) {
	
	if (!CheckPerm(*cdir, INode::IWRITE, ffor_flag)) {
		IsError();
		return;
	}
	if (!checkPathName(dirName)) {
		return;
	}
	arg[1] = INode::IFDIR;
	fileManager->Creat();
	IsError();
}

void User::Ls() {
	vector<string> show;
	ls.clear();
	show = fileManager->Ls();
	if (IsError()) {
		return;
	}
	for (int i = 0;i < show.size();i = i + 2) {
		if (show[i + 1] == "\tdir") {
			g_Operate.textcolor(9);
		}
		else if (show[i + 1] == "\tfile") {
			g_Operate.textcolor(6);
		}
		cout << show[i];
		cout << show[i + 1] << endl;
	}
	cout << endl;
	//cout << ls << endl;
}

void User::Cd(string dirName) {
	if (!checkPathName(dirName)) {
		return;
	}
	fileManager->ChDir();
	IsError();
}

void User::Create(string fileName, string mode) {
	CheckPerm(*cdir, INode::IWRITE, ffor_flag);
	if (IsError()) {
		return;
	}
	if (!checkPathName(fileName)) {
		return;
	}
	int md = INodeMode(mode); //将字符类型的mode -rw 转换为数字类型
	if (md == 0) {
		cout << "this mode is undefined !  \n";
		return;
	}

	arg[1] = md;
	fileManager->Creat();
	IsError();
}

void User::Delete(string fileName) {
	if (!checkPathName(fileName)) {
		return;
	}
	CheckPerm(*cdir, INode::IWRITE, ffor_flag);
	if (IsError()) {
		return;
	}
	fileManager->UnLink();
	IsError();
}

bool User::Open(string fileName, string mode) {
	if (!checkPathName(fileName)) {
		return false;
	}
	int md = FileMode(mode);
	if (md == 0) {
		cout << "this mode is undefined ! \n";
		return false;
	}

	arg[1] = md;
	fileManager->Open();
	if (IsError())
		return false;
	return true;
	//cout << "open success, return fd=" << ar0[EAX] << endl;
}

void User::Close(string sfd) {
	if (sfd.empty() || !isdigit(sfd.front())) {
		cout << "parameter fd can't be empty or be nonnumeric ! \n";
		return;
	}
	arg[0] = stoi(sfd);;
	fileManager->Close();
	IsError();
}

void User::Seek(string sfd, string offset, string origin) {
	if (sfd.empty() || !isdigit(sfd.front())) {
		cout << "parameter fd can't be empty or be nonnumeric ! \n";
		return;
	}
	if (offset.empty()) {
		cout << "parameter offset can't be empty ! \n";
		return;
	}
	if (origin.empty() || !isdigit(origin.front())) {
		cout << "parameter origin can't be empty or be nonnumeric ! \n";
		return;
	}
	arg[0] = stoi(sfd);
	arg[1] = stoi(offset);
	arg[2] = stoi(origin);
	fileManager->Seek();
	IsError();
}

void User::WritefromFile(string sfd, string inFile, string size) {
	if (sfd.empty() || !isdigit(sfd.front())) {
		cout << "parameter fd can't be empty or be nonnumeric ! \n";
		return;
	}
	int fd = stoi(sfd);

	int usize;
	if (size.empty() || (usize = stoi(size)) < 0) {
		cout << "parameter size must be greater or equal than 0 ! \n";
		return;
	}

    char *buffer = new char[usize];
	fstream fin(inFile, ios::in | ios::binary);
	if (!fin) {
		cout << "file " << inFile << " open failed ! \n";
		return;
	}
    fin.read(buffer, usize); //从文件中读取usize字节到临时申请的缓冲区
	fin.close();
	//cout << "fd = " << fd << " inFile = " << inFile << " size = " << usize << "\n";
	arg[0] = fd;
	arg[1] = (long)buffer;
	arg[2] = usize;
    fileManager->WritefromFile();

    if (IsError())
        return;
    cout << "write " << ar0[User::EAX] << "bytes success !" << endl;
    delete[]buffer;
}

void User::WritefromConsole(string sfd, string content)
{
    if (sfd.empty() || !isdigit(sfd.front())) {
        cout << "parameter fd can't be empty or be nonnumeric ! \n";
        return;
    }
    int fd = stoi(sfd);
    int len = 0;
    char* buffer = new char[BufferManager::BUFFER_SIZE];

    
    if (content.length() == 0)
    {
        cout << "please input the content you want to write:\n";
        getline(cin, content);
    }
    strcpy(buffer, content.c_str());
    len = content.length();   
    arg[0] = fd;
    arg[1] = (long)buffer;
    arg[2] = len;
    fileManager->WritefromFile();

	if (IsError())
		return;
	cout << "write " << ar0[User::EAX] << "bytes success !" << endl;
	delete[]buffer;
}

char* User::Read(string sfd, string outFile, string size) {
	if (sfd.empty() || !isdigit(sfd.front())) {
		cout << "parameter fd can't be empty or be nonnumeric ! \n";
		return 0;
	}
	int fd = stoi(sfd);

	int usize;
	if (size.empty() || !isdigit(size.front()) || (usize = stoi(size)) < 0) {
		cout << "parameter size is not right \n";
		return 0;
	}
	char* buffer = new char[usize];
	arg[0] = fd;
	arg[1] = (long)buffer;
	arg[2] = usize;
	fileManager->Read();
	if (IsError())
		return 0;
	if (pw_uid == -1) {
		return buffer;
	}
	cout << "read " << ar0[User::EAX] << " bytes success : \n";
	if (outFile.empty()) {
		for (unsigned int i = 0; i < ar0[User::EAX]; ++i) {
			cout << (char)buffer[i];
		}
		cout << " \n";
		return buffer;
	}
	fstream fout(outFile, ios::out | ios::binary);
	if (!fout) {
		cout << "file " << outFile << " open failed ! \n";
		return 0;
	}
	fout.write(buffer, ar0[User::EAX]);
	fout.close();
	cout << "read to " << outFile << " done ! \n";
	return buffer;
}

int User::INodeMode(string mode) {
	int md = 0;
	if (mode.find("-r") != string::npos) {
		md |= INode::IREAD;
	}
	if (mode.find("-w") != string::npos) {
		md |= INode::IWRITE;
	}
	if (mode.find("-rw") != string::npos) {
		md |= (INode::IREAD | INode::IWRITE);
	}
	return md;
}

void User::Copy(string srcFileName, string destFileName)
{
	this->Create(destFileName,"-rw");
	Open(srcFileName, "-r");
	string src_fp = to_string(ar0[EAX]);
	int size = 512;  //512 need to be changed
	char* buffer = Read(src_fp, "",to_string(size));
	Open(destFileName, "-w");
	int fd= ar0[EAX];
	arg[0] = fd;
	arg[1] = (long)buffer;
	arg[2] = size;

	fileManager->WritefromFile();
	//system("cls");
	Close(to_string(fd));
	Close(src_fp);
}

void User::Move(string srcFileName, string destFileName)
{//先跑到目标目录下,添加一项
	//然后回来把目录的这一项给删除掉,不要删文件
	if (!checkPathName(srcFileName)) {
		return;
	}
	if (!checkPathName(destFileName)) {
		return;
	}
	//调用Filemanager的文件复制
	fileManager->Mov(srcFileName);
	IsError();
}

void User::Pwd()
{
	cout << this->curDirPath<<endl;
}

int User::FileMode(string mode) {
	int md = 0;
	if (mode.find("-r") != string::npos) {
		md |= File::FREAD;
	}
	if (mode.find("-w") != string::npos) {
		md |= File::FWRITE;
	}
	if (mode.find("-rw") != string::npos) {
		md |= (File::FREAD | File::FWRITE);
	}
	return md;
}

bool User::checkPathName(string path) {
	// FileManager 中函数不判断参数的合法性，最好在User中过滤，
	// 暂不考虑过多的参数不合法情况
	if (path.empty()) {
		cout << "parameter path can't be empty ! \n";
		return false;
	}

	if (path.substr(0, 2) != "..") {
		dirp = path;
	}
	else {
		string pre = curDirPath;
		unsigned int p = 0;
		//可以多重相对路径 .. ../ ../.. ../../
		for (; pre.length() > 3 && p < path.length() && path.substr(p, 2) == ".."; ) {
			pre.pop_back();
			pre.erase(pre.find_last_of('/') + 1);
			p += 2;
			p += p < path.length() && path[p] == '/';
		}
		dirp = pre + path.substr(p);
	}

	if (dirp.length() > 1 && dirp.back() == '/') {
		dirp.pop_back();
	}

	for (unsigned int p = 0, q = 0; p < dirp.length(); p = q + 1) {
		q = dirp.find('/', p);
		q = min(q, (unsigned int)dirp.length());
		if (q - p > DirectoryEntry::DIRSIZ) {
			cout << "the fileName or dirPath can't be greater than 28 size ! \n";
			return false;
		}
	}
	return true;
}

bool User::IsError() {
	if (u_error != U_NOERROR) {
		cout << "errno = " << u_error;
		EchoError(u_error);
		u_error = U_NOERROR;
		return true;
	}
	return false;
}

void User::EchoError(enum ErrorCode err) {
	string estr;
	switch (err) {
	case User::U_NOERROR:
		estr = " No u_error ";
		break;
	case User::U_ENOENT:
		estr = " No such file or directory ";
		break;
	case User::U_EBADF:
		estr = " Bad file number ";
		break;
	case User::U_EACCES:
		estr = " Permission denied ";
		break;
	case User::U_ENOTDIR:
		estr = " Not a directory ";
		break;
	case User::U_ENFILE:
		estr = " File table overflow ";
		break;
	case User::U_EMFILE:
		estr = " Too many open files ";
		break;
	case User::U_EFBIG:
		estr = " File too large ";
		break;
	case User::U_ENOSPC:
		estr = " No space left on device ";
		break;
	case User::U_NOTFILE:
		estr = " Can't open directory ";
	default:
		break;
	}
	cout << estr << endl;
}

void User::Link(string srcFileName, string destFileName) {
	if (!checkPathName(srcFileName)) {
		return;
	}
	if (!checkPathName(destFileName)) {
		return;
	}
	//调用Filemanager的文件勾连
	fileManager->Link(srcFileName);
	IsError();
}