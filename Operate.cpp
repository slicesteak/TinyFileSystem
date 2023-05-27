#include "Operate.h"

extern DeviceDriver g_DeviceDriver;
extern BufferManager g_BufferManager;
extern OpenFileTable g_OpenFileTable; 
extern SuperBlock g_SuperBlock; 
extern FileSystem g_FileSystem;
extern INodeTable g_INodeTable;
extern FileManager g_FileManager;
extern User g_User;
extern int ffor_flag; //初始化标志
int testNo = 0;
void Operate::man(string command) {

	static string man =
		"Command       :  man -显示在线帮助手册 \n"
		"Description   :  帮助用户使用相应命令 \n"
		"Usage         :  man [命令] \n"
		"Parameter     :  [命令] 如下：  \n"
		"                 man          :  手册 \n"
		"                 fformat      :  格式化 \n"
		"                 exit         :  正确退出 \n"
		"                 mkdir        :  新建目录 \n"
		"                 cd           :  改变目录 \n"
		"                 ls           :  列出目录及文件 \n"
		"                 create       :  新建文件 \n"
		"                 delete       :  删除文件 \n"
		"                 open         :  打开文件 \n"
		"                 close        :  关闭文件 \n"
		"                 seek         :  移动读写指针 \n"
		"                 write        :  写入文件 \n"
		"                 read         :  读取文件 \n"
		"                 mv           :  移动文件 \n"
		"                 cp           :  复制文件 \n"
		"                 link         :  链接文件 \n"
		"                 at|autoTest  :  自动测试 \n"
		"Usage Demo    :  man mkdir my_dir\n"
		;

	static string fformat =
		"Command       :  fformat -进行文件系统格式化 \n"
		"Description   :  将整个文件系统进行格式化，即清空所有文件及目录! \n"
		"Usage         :  fformat \n"
		"Parameter     :  无 \n"
		"Usage Demo    :  fformat \n"
		;

	static string exit =
		"Command       :  exit -退出文件系统 \n"
		"Description   :  若要退出程序，最好通过exit命令。此时正常退出会调用析构函数，\n"
		"              :  若有在内存中未更新到磁盘上的缓存会及时更新，保证正确性。若点 \n"
		"              :  击窗口关闭按钮，属于给当前程序发信号强制退出，不会调用析构函 \n"
		"              :  数，未写回部分信息，再次启动时可能出现错误! \n"
		"Usage         :  exit \n"
		"Parameter     :  无 \n"
		"Usage Demo    :  exit \n"
		;

	static string mkdir =
		"Command       :  mkdir -建立目录 \n"
		"Description   :  新建一个目录。若出现错误，会有相应错误提示! \n"
		"Usage         :  mkdir <目录名> \n"
		"Parameter     :  <目录名> 可以是相对路径，也可以是绝对路径 \n"
		"Usage Demo    :  mkdir dirName \n"
		"                 mkdir ../dirName \n"
		"                 mkdir ../../dirName \n"
		"                 mkdir /dirName \n"
		"                 mkdir /dir1/dirName \n"
		"Error Avoided :  目录名过长，目录路径不存在，目录超出根目录等 \n"
		;

	static string ls =
		"Command       :  ls -列目录内容 \n"
		"Description   :  列出当前目录中包含的文件名或目录名。若出现错误，会有相应错误提示! \n"
		"Usage         :  ls \n"
		"Parameter     :  无 \n"
		"Usage Demo    :  ls \n"
		;

	static string cd =
		"Command       :  cd -改变当前目录 \n"
		"Description   :  改变当前工作目录。若出现错误，会有相应错误提示! \n"
		"Usage         :  cd <目录名> \n"
		"Parameter     :  <目录名>默认为当前目录；\n"
		"                 <目录名> 可以是相对路径，也可以是绝对路径 \n"
		"Usage Demo    :  ls \n"
		"                 ls ../dirName \n"
		"                 ls ../../dirName \n"
		"                 ls /dirName \n"
		"                 ls /dir1/dirName \n"
		"Error Avoided :  目录名过长，目录路径不存在，目录超出根目录等 \n"
		;

	static string create =
		"Command       :  create -新建文件 \n"
		"Description   :  新建一个文件。若出现错误，会有相应错误提示! \n"
		"Usage         :  create <文件名> \n"
		"Parameter     :  <文件名> 可以包含相对路径或绝对路径 \n"
		"Usage Demo    :  create newFileName \n"
		"                 create ../newFileName \n"
		"                 create ../../newFileName \n"
		"                 create /newFileName \n"
		"                 create /dir1/newFileName \n"
		"Error Avoided :  文件名过长，目录路径不存在，目录超出根目录等 \n"
		;

	static string delet =
		"Command       :  delete -删除文件 \n"
		"Description   :  删除一个文件。若出现错误，会有相应错误提示! \n"
		"Usage         :  delete <文件名> \n"
		"Parameter     :  <文件名> 可以包含相对路径或绝对路径 \n"
		"Usage Demo    :  delete fileName \n"
		"                 delete ../fileName \n"
		"                 delete ../../fileName \n"
		"                 delete /fileName \n"
		"                 delete /dir1/fileName \n"
		"Error Avoided :  文件名过长，目录路径不存在，目录超出根目录等 \n"
		;

	static string open =
		"Command       :  open -打开文件 \n"
		"Description   :  类Unix|Linux函数open，打开一个文件。若要进行文件的读写必须先open。\n"
		"                 若出现错误，会有相应错误提示! \n"
		"Usage         :  open <文件名> <选项> \n"
		"Parameter     :  <文件名> 可以包含相对路径或绝对路径 \n"
		"                 <选项> -r 只读属性 \n"
		"                 <选项> -w 只写属性 \n"
		"                 <选项> -rw == -r -w 读写属性 \n"
		"Usage Demo    :  open fileName -r \n"
		"                 open ../fileName -w \n"
		"                 open ../../fileName -rw \n"
		"                 open /fileName -r -w \n"
		"                 open /dir1/fileName -rw \n"
		"Error Avoided :  文件名过长，目录路径不存在，目录超出根目录等 \n"
		;

	static string close =
		"Command       :  close -关闭文件 \n"
		"Description   :  类Unix|Linux函数close，关闭一个文件。可以对已经打开的文件进行关闭 \n"
		"                 若出现错误，会有相应错误提示! \n"
		"Usage         :  close <file descriptor> \n"
		"Parameter     :  <file descriptor> 文件描述符 \n"
		"Usage Demo    :  close 1 \n"
		"Error Avoided :  文件描述符不存在或超出范围 \n"
		;

	static string seek =
		"Command       :  seek -写入文件 \n"
		"Description   :  类Unix|Linux函数fseek，写入一个已经打开的文件中。若出现错误，会有相应错误提示! \n"
		"Usage         :  seek <file descriptor> <offset> <origin> \n"
		"Parameter     :  <file descriptor> open返回的文件描述符 \n"
		"                 <offset> 指定从 <origin> 开始的偏移量 可正可负 \n"
		"                 <origin> 指定起始位置 可为0(SEEK_SET), 1(SEEK_CUR), 2(SEEK_END) \n"
		"Usage Demo    :  seek 1 500 0 \n"
		"Error Avoided :  文件描述符不存在或超出范围，未正确指定参数 \n"
		;

	static string write =
		"Command       :  write -写入文件 \n"
		"Description   :  类Unix|Linux函数write，写入一个已经打开的文件中。若出现错误，会有相应错误提示! \n"
		"Usage         :  write <file descriptor> <[-f <InFileName> <size>]|[-c <content>]> \n"
		"Parameter     :  <file descriptor> open返回的文件描述符 \n"
		"                 <InFileName> 指定写入内容为文件InFileName中的内容 \n"
		"                 <size> 指定写入字节数，大小为 <size> \n"
		"                 <content> 想要直接写入的内容 \n"
		"Usage Demo    :  write 1 InFileName 123 \n"
		"Error Avoided :  文件描述符不存在或超出范围，未正确指定参数 \n"
		;

	static string read =
		"Command       :  read -读取文件 \n"
		"Description   :  类Unix|Linux函数read，从一个已经打开的文件中读取。若出现错误，会有相应错误提示! \n"
		"Usage         :  read <file descriptor> <size> \n"
		"Parameter     :  <file descriptor> open返回的文件描述符 \n"
		"                 <size> 指定读取字节数，大小为 <size> \n"
		"Usage Demo    :  read 1 123 \n"
		"Error Avoided :  文件描述符不存在或超出范围，未正确指定参数 \n"
		;

	static string cp =
		"Command       :  cp -复制文件 \n"
		"Description   :  类Unix|Linux函数cp，复制一个文件到另一个地方! \n"
		"Usage         :  cp <src FileName> <dest FileName> \n"
		"Parameter     :  <src FileName> 源文件名 \n"
		"                 <dest FileName> 目标文件名\n"
		"Usage Demo    :  cp /hello /home/hello \n"
		"Error Avoided :  未正确指定参数 \n"
		;

	static string mv =
		"Command       :  mv -移动文件 \n"
		"Description   :  类Unix|Linux函数cp，移动一个文件到另一个地方! \n"
		"Usage         :  mv <src FileName> <dest FileName> \n"
		"Parameter     :  <src FileName> 源文件名 \n"
		"                 <dest FileName> 目标文件名\n"
		"Usage Demo    :  mv /hello /home/hello \n"
		"Error Avoided :  未正确指定参数 \n"
		;

	static string link =
		"Command       :  link -链接文件 \n"
		"Description   :  创建文件的硬链接\n"
		"Usage         :  link <srcFileName> <destDir> \n"
		"Parameter     :  <srcFileName> 源文件名 \n"
		"                 <destDir> 目标文件夹名\n"
		"Usage Demo    :  link test.txt /etc \n"
		;

	static string autoTest =
		"Command       :  autoTest -自动测试 \n"
		"Description   :  帮助测试，在系统启动初期帮助测试。测试不一定所有命令都是正确的，但是系统具有容错性，\n"
		"              :  不会使系统异常。\n"
		"Usage         :  autoTest | at \n"
		"Parameter     :  无 \n"
		"Usage Demo    :  at \n"
		;


	static unordered_map<string, string*>manMap({
		{ "man", &man },
		{ "at", &autoTest },
		{ "fformat", &fformat },
		{ "exit", &exit },
		{ "mkdir", &mkdir },
		{ "cd", &cd },
		{ "ls", &ls },
		{ "create", &create },
		{ "delete", &delet },
		{ "open", &open },
		{ "close", &close },
		{ "seek", &seek },
		{ "write", &write },
		{ "read", &read },
		{ "cp", &cp },
		{ "mv", &mv },
		{ "link", &link },
		{ "autoTest", &autoTest },
		});

	auto it = manMap.find(command);
	if (it == manMap.end()) {
		cout << "shell : " << command << " : don't find this commond \n";
		return;
	}
	cout << *it->second;
}

bool Operate::autoTest(string& cmd, User* user) {
	if (user->pw_uid == 0) {
		//执行root的自动测试命令
		static  const char* testCmds0[] = {
			"cd home/user1",
			"open apple2 -rw",//管理员可以打开
			"read 4 100",
			"close 4",
			"link apple2 /etc",
			"open apple2 -rw",
			"write 4 -c myapple",
			"close 4",
			"cd /etc",
			"open apple2 -rw",
			"read 4 7",
			"close 4",
			"fformat"
		};
		int cmdNums = sizeof(testCmds0) / sizeof(char*);
		cmd = testCmds0[testNo % cmdNums];
		return ++testNo <= cmdNums;
	}
	else if (user->pw_uid == 1) {
		//执行user1的自动测试命令
		static  const char* testCmds1[] = {
			"create apple1",
			"open apple1 -rw",
			"write 1 -c apple123456789",
			"close 1",
			"ls",
			"open apple1 -rw",
			"read 1 100",
			"close 1",
			"ls",
			"create apple2",
			"open apple2 -rw",
			"write 2 -c helloworld!",
			"close 2",
			"ls",
			//加seek这里加
			"open apple2 -rw",
			"seek 2 5 0",
			"read 2 30",
			"close 2 ",
			"mkdir dir1",
			"cd ../../",
			"cd /home/user1",
			"pwd",//查地址
			"ls",
			"mv apple1 /home/user1/dir1",
			"cp apple2 /etc/orange",
			"cd /etc",
			"ls",
			"cd /home/user1/dir1",
			"ls",
			"delete apple1",
			"ls"
		};
		int cmdNums = sizeof(testCmds1) / sizeof(char*);
		cmd = testCmds1[testNo % cmdNums];
		return ++testNo <= cmdNums;
	}
	else {
		//执行user2的自动测试命令
		static  const char* testCmds2[] = {
			"cd ../user1",
			"ls",
			"open apple2 -rw",//没有权限，不能打开
			"open apple2 -r",//只读打开
			"read 4 100",
			"close 4"
		};
		int cmdNums = sizeof(testCmds2) / sizeof(char*);
		cmd = testCmds2[testNo % cmdNums];
		return ++testNo <= cmdNums;
	}
}

vector<string> Operate::analyse(string edits) {
	vector<string> dp;
	string command = {};//单个命令
	if (edits.size() == 0) {
		dp.push_back(command);
		return dp;
	}
	for (int i = 0;i < edits.size();i++) {
		if (edits[i] == ' ') {
			if (command.size() != 0) {
				//长度不为0
				dp.push_back(command);
				command = "";
			}
		}
		else {
			command = command + edits[i];
		}
	}
	if (command.size() != 0) {
		//长度不为0
		dp.push_back(command);
		command = "";
	}
	return dp;
}

//经过处理的用户输入
string Operate::User_input() {
	string command;//用户输入的命令
	char coin;//每次输入的单个字符
	while (cin.get(coin)) {
		//输入命令，以回车结束
		if (coin == '\n')
			break;
		command = command + coin;
	}
	return command;
}

string Operate::Arg_input(int &autoFlag, User* user) {
	string line = "man";
	textcolor(2);//设置绿色
	cout << "Unix_FileSystem ";
	textcolor(3);//设置浅蓝色
	cout << user->curDirPath;
	if (user->pw_name == "root") {
		cout << "$ ";
	}
	else {
		cout << "# ";
	}
	textcolor(7);//设置白色
	if (autoFlag) {
		if (autoTest(line, user)) {
			cout << line;
			getchar();
		}
		else {
			cout << "autoTest finished ... \n";
			testNo = 0;
			autoFlag = 0;
			line = "";
		}
	}
	else {
		//getline(cin, line);
		line = User_input();
	}
	return line;
}

void Operate::display() {
	ffor_flag = 1;
	if (g_SuperBlock.s_ninode == 100)
	{
		
		g_User.init();
		
	}
	g_User.Login();
	User* user = &g_User;
	ffor_flag = 0;
	string line = "man";
	vector<string> args;
	string cmd, arg1, arg2, arg3;
	int autoFlag = 0;
	cout << "++++++++++++++++++++ Unix文件系统模拟 ++++++++++++++++++++" << endl;
	cout << endl;
	man(arg1.empty() ? "man" : arg1);
	while (1) {
		if (user->pw_uid == -1) {
			ffor_flag = 1;
			g_User.Login();
			arg1 = "";
			man(arg1.empty() ? "man" : arg1);
			user = &g_User;
			ffor_flag = 0;
		}
		line = Arg_input(autoFlag, user);//命令输入
		args = analyse(line);
		cmd = args[0];
		arg1 = args.size() > 1 ? args[1] : "";
		arg2 = args.size() > 2 ? args[2] : "";
		arg3 = args.size() > 3 ? args[3] : "";
		if (cmd == "man") {
			man(arg1.empty() ? "man" : arg1);
		}
		else if (cmd == "fformat") {
			if (user->pw_uid != 0) {
				cout << "permission denied" << endl;
			}
			else {
				g_OpenFileTable.Format();
				g_INodeTable.Format();
				g_BufferManager.FormatBuffer();
				g_FileSystem.FormatDevice();
				exit(0);
			}
		}
		else if (cmd == "exit") {
			exit(0);
		}
		else if (cmd == "at" || cmd == "autoTest") {
			autoFlag = 1;
			cout << "autoTest begin ... \njust press enter to continue ... \n";
		}
		else if (cmd == "mkdir") {
			user->Mkdir(arg1);
		}
		else if (cmd == "ls") {
			user->Ls();
		}
		else if (cmd == "cd") {
			user->Cd(arg1);
		}
		else if (cmd == "create") {
			//create 没有需要模式的必要
			//user->Create(arg1, arg2 + arg3);
			user->Create(arg1, "-rw");
		}
		else if (cmd == "delete") {
			user->Delete(arg1);
		}
		else if (cmd == "open") {
			if (user->Open(arg1, line))
				cout << "open success, return fd= " << user->ar0[user->EAX] << endl;
		}
		else if (cmd == "close") {
			user->Close(arg1);
		}
		else if (cmd == "seek") {
			user->Seek(arg1, arg2, arg3);
		}
		else if (cmd == "read") {
			if (arg2 == "-o")
				delete[] user->Read(arg1, arg3, args[4]);
			else
				delete[] user->Read(arg1, "", arg2);
		}
		else if (cmd == "write") {

			if (arg2 == "-c")
				user->WritefromConsole(arg1, arg3);
			else if (arg2 == "-f")
				user->WritefromFile(arg1, arg3, args[4]);
		}
		else if (cmd == "logout") {
			g_User.Logout();
		}
		else if (cmd == "cp") {
			g_User.Copy(arg1, arg2);
		}
		else if (cmd == "mv") {
			user->Move(arg1, arg2);
		}
		else if (cmd == "pwd") {  //copy files
			user->Pwd();
		}
		else if (cmd == "link") {
			user->Link(arg1, arg2);
		}
		else if (cmd != "") {
			cout << "shell : " << cmd << " : don't find this commond \n";
		}
	}
}

//设置文本输出颜色
void Operate::textcolor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
//1	    深蓝色
//2	    深绿色
//3	    深青色
//4	    深红色
//5	    深粉色
//6	    黄色
//7	    深白色
//8	    灰色
//9	    浅蓝色
//10	浅绿色
//11	浅青色
//12	浅红色
//13	浅粉色
//14	浅黄色
//15	浅白色
