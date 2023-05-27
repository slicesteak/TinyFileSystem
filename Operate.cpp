#include "Operate.h"

extern DeviceDriver g_DeviceDriver;
extern BufferManager g_BufferManager;
extern OpenFileTable g_OpenFileTable; 
extern SuperBlock g_SuperBlock; 
extern FileSystem g_FileSystem;
extern INodeTable g_INodeTable;
extern FileManager g_FileManager;
extern User g_User;
extern int ffor_flag; //��ʼ����־
int testNo = 0;
void Operate::man(string command) {

	static string man =
		"Command       :  man -��ʾ���߰����ֲ� \n"
		"Description   :  �����û�ʹ����Ӧ���� \n"
		"Usage         :  man [����] \n"
		"Parameter     :  [����] ���£�  \n"
		"                 man          :  �ֲ� \n"
		"                 fformat      :  ��ʽ�� \n"
		"                 exit         :  ��ȷ�˳� \n"
		"                 mkdir        :  �½�Ŀ¼ \n"
		"                 cd           :  �ı�Ŀ¼ \n"
		"                 ls           :  �г�Ŀ¼���ļ� \n"
		"                 create       :  �½��ļ� \n"
		"                 delete       :  ɾ���ļ� \n"
		"                 open         :  ���ļ� \n"
		"                 close        :  �ر��ļ� \n"
		"                 seek         :  �ƶ���дָ�� \n"
		"                 write        :  д���ļ� \n"
		"                 read         :  ��ȡ�ļ� \n"
		"                 mv           :  �ƶ��ļ� \n"
		"                 cp           :  �����ļ� \n"
		"                 link         :  �����ļ� \n"
		"                 at|autoTest  :  �Զ����� \n"
		"Usage Demo    :  man mkdir my_dir\n"
		;

	static string fformat =
		"Command       :  fformat -�����ļ�ϵͳ��ʽ�� \n"
		"Description   :  �������ļ�ϵͳ���и�ʽ��������������ļ���Ŀ¼! \n"
		"Usage         :  fformat \n"
		"Parameter     :  �� \n"
		"Usage Demo    :  fformat \n"
		;

	static string exit =
		"Command       :  exit -�˳��ļ�ϵͳ \n"
		"Description   :  ��Ҫ�˳��������ͨ��exit�����ʱ�����˳����������������\n"
		"              :  �������ڴ���δ���µ������ϵĻ���ἰʱ���£���֤��ȷ�ԡ����� \n"
		"              :  �����ڹرհ�ť�����ڸ���ǰ�����ź�ǿ���˳���������������� \n"
		"              :  ����δд�ز�����Ϣ���ٴ�����ʱ���ܳ��ִ���! \n"
		"Usage         :  exit \n"
		"Parameter     :  �� \n"
		"Usage Demo    :  exit \n"
		;

	static string mkdir =
		"Command       :  mkdir -����Ŀ¼ \n"
		"Description   :  �½�һ��Ŀ¼�������ִ��󣬻�����Ӧ������ʾ! \n"
		"Usage         :  mkdir <Ŀ¼��> \n"
		"Parameter     :  <Ŀ¼��> ���������·����Ҳ�����Ǿ���·�� \n"
		"Usage Demo    :  mkdir dirName \n"
		"                 mkdir ../dirName \n"
		"                 mkdir ../../dirName \n"
		"                 mkdir /dirName \n"
		"                 mkdir /dir1/dirName \n"
		"Error Avoided :  Ŀ¼��������Ŀ¼·�������ڣ�Ŀ¼������Ŀ¼�� \n"
		;

	static string ls =
		"Command       :  ls -��Ŀ¼���� \n"
		"Description   :  �г���ǰĿ¼�а������ļ�����Ŀ¼���������ִ��󣬻�����Ӧ������ʾ! \n"
		"Usage         :  ls \n"
		"Parameter     :  �� \n"
		"Usage Demo    :  ls \n"
		;

	static string cd =
		"Command       :  cd -�ı䵱ǰĿ¼ \n"
		"Description   :  �ı䵱ǰ����Ŀ¼�������ִ��󣬻�����Ӧ������ʾ! \n"
		"Usage         :  cd <Ŀ¼��> \n"
		"Parameter     :  <Ŀ¼��>Ĭ��Ϊ��ǰĿ¼��\n"
		"                 <Ŀ¼��> ���������·����Ҳ�����Ǿ���·�� \n"
		"Usage Demo    :  ls \n"
		"                 ls ../dirName \n"
		"                 ls ../../dirName \n"
		"                 ls /dirName \n"
		"                 ls /dir1/dirName \n"
		"Error Avoided :  Ŀ¼��������Ŀ¼·�������ڣ�Ŀ¼������Ŀ¼�� \n"
		;

	static string create =
		"Command       :  create -�½��ļ� \n"
		"Description   :  �½�һ���ļ��������ִ��󣬻�����Ӧ������ʾ! \n"
		"Usage         :  create <�ļ���> \n"
		"Parameter     :  <�ļ���> ���԰������·�������·�� \n"
		"Usage Demo    :  create newFileName \n"
		"                 create ../newFileName \n"
		"                 create ../../newFileName \n"
		"                 create /newFileName \n"
		"                 create /dir1/newFileName \n"
		"Error Avoided :  �ļ���������Ŀ¼·�������ڣ�Ŀ¼������Ŀ¼�� \n"
		;

	static string delet =
		"Command       :  delete -ɾ���ļ� \n"
		"Description   :  ɾ��һ���ļ��������ִ��󣬻�����Ӧ������ʾ! \n"
		"Usage         :  delete <�ļ���> \n"
		"Parameter     :  <�ļ���> ���԰������·�������·�� \n"
		"Usage Demo    :  delete fileName \n"
		"                 delete ../fileName \n"
		"                 delete ../../fileName \n"
		"                 delete /fileName \n"
		"                 delete /dir1/fileName \n"
		"Error Avoided :  �ļ���������Ŀ¼·�������ڣ�Ŀ¼������Ŀ¼�� \n"
		;

	static string open =
		"Command       :  open -���ļ� \n"
		"Description   :  ��Unix|Linux����open����һ���ļ�����Ҫ�����ļ��Ķ�д������open��\n"
		"                 �����ִ��󣬻�����Ӧ������ʾ! \n"
		"Usage         :  open <�ļ���> <ѡ��> \n"
		"Parameter     :  <�ļ���> ���԰������·�������·�� \n"
		"                 <ѡ��> -r ֻ������ \n"
		"                 <ѡ��> -w ֻд���� \n"
		"                 <ѡ��> -rw == -r -w ��д���� \n"
		"Usage Demo    :  open fileName -r \n"
		"                 open ../fileName -w \n"
		"                 open ../../fileName -rw \n"
		"                 open /fileName -r -w \n"
		"                 open /dir1/fileName -rw \n"
		"Error Avoided :  �ļ���������Ŀ¼·�������ڣ�Ŀ¼������Ŀ¼�� \n"
		;

	static string close =
		"Command       :  close -�ر��ļ� \n"
		"Description   :  ��Unix|Linux����close���ر�һ���ļ������Զ��Ѿ��򿪵��ļ����йر� \n"
		"                 �����ִ��󣬻�����Ӧ������ʾ! \n"
		"Usage         :  close <file descriptor> \n"
		"Parameter     :  <file descriptor> �ļ������� \n"
		"Usage Demo    :  close 1 \n"
		"Error Avoided :  �ļ������������ڻ򳬳���Χ \n"
		;

	static string seek =
		"Command       :  seek -д���ļ� \n"
		"Description   :  ��Unix|Linux����fseek��д��һ���Ѿ��򿪵��ļ��С������ִ��󣬻�����Ӧ������ʾ! \n"
		"Usage         :  seek <file descriptor> <offset> <origin> \n"
		"Parameter     :  <file descriptor> open���ص��ļ������� \n"
		"                 <offset> ָ���� <origin> ��ʼ��ƫ���� �����ɸ� \n"
		"                 <origin> ָ����ʼλ�� ��Ϊ0(SEEK_SET), 1(SEEK_CUR), 2(SEEK_END) \n"
		"Usage Demo    :  seek 1 500 0 \n"
		"Error Avoided :  �ļ������������ڻ򳬳���Χ��δ��ȷָ������ \n"
		;

	static string write =
		"Command       :  write -д���ļ� \n"
		"Description   :  ��Unix|Linux����write��д��һ���Ѿ��򿪵��ļ��С������ִ��󣬻�����Ӧ������ʾ! \n"
		"Usage         :  write <file descriptor> <[-f <InFileName> <size>]|[-c <content>]> \n"
		"Parameter     :  <file descriptor> open���ص��ļ������� \n"
		"                 <InFileName> ָ��д������Ϊ�ļ�InFileName�е����� \n"
		"                 <size> ָ��д���ֽ�������СΪ <size> \n"
		"                 <content> ��Ҫֱ��д������� \n"
		"Usage Demo    :  write 1 InFileName 123 \n"
		"Error Avoided :  �ļ������������ڻ򳬳���Χ��δ��ȷָ������ \n"
		;

	static string read =
		"Command       :  read -��ȡ�ļ� \n"
		"Description   :  ��Unix|Linux����read����һ���Ѿ��򿪵��ļ��ж�ȡ�������ִ��󣬻�����Ӧ������ʾ! \n"
		"Usage         :  read <file descriptor> <size> \n"
		"Parameter     :  <file descriptor> open���ص��ļ������� \n"
		"                 <size> ָ����ȡ�ֽ�������СΪ <size> \n"
		"Usage Demo    :  read 1 123 \n"
		"Error Avoided :  �ļ������������ڻ򳬳���Χ��δ��ȷָ������ \n"
		;

	static string cp =
		"Command       :  cp -�����ļ� \n"
		"Description   :  ��Unix|Linux����cp������һ���ļ�����һ���ط�! \n"
		"Usage         :  cp <src FileName> <dest FileName> \n"
		"Parameter     :  <src FileName> Դ�ļ��� \n"
		"                 <dest FileName> Ŀ���ļ���\n"
		"Usage Demo    :  cp /hello /home/hello \n"
		"Error Avoided :  δ��ȷָ������ \n"
		;

	static string mv =
		"Command       :  mv -�ƶ��ļ� \n"
		"Description   :  ��Unix|Linux����cp���ƶ�һ���ļ�����һ���ط�! \n"
		"Usage         :  mv <src FileName> <dest FileName> \n"
		"Parameter     :  <src FileName> Դ�ļ��� \n"
		"                 <dest FileName> Ŀ���ļ���\n"
		"Usage Demo    :  mv /hello /home/hello \n"
		"Error Avoided :  δ��ȷָ������ \n"
		;

	static string link =
		"Command       :  link -�����ļ� \n"
		"Description   :  �����ļ���Ӳ����\n"
		"Usage         :  link <srcFileName> <destDir> \n"
		"Parameter     :  <srcFileName> Դ�ļ��� \n"
		"                 <destDir> Ŀ���ļ�����\n"
		"Usage Demo    :  link test.txt /etc \n"
		;

	static string autoTest =
		"Command       :  autoTest -�Զ����� \n"
		"Description   :  �������ԣ���ϵͳ�������ڰ������ԡ����Բ�һ�������������ȷ�ģ�����ϵͳ�����ݴ��ԣ�\n"
		"              :  ����ʹϵͳ�쳣��\n"
		"Usage         :  autoTest | at \n"
		"Parameter     :  �� \n"
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
		//ִ��root���Զ���������
		static  const char* testCmds0[] = {
			"cd home/user1",
			"open apple2 -rw",//����Ա���Դ�
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
		//ִ��user1���Զ���������
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
			//��seek�����
			"open apple2 -rw",
			"seek 2 5 0",
			"read 2 30",
			"close 2 ",
			"mkdir dir1",
			"cd ../../",
			"cd /home/user1",
			"pwd",//���ַ
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
		//ִ��user2���Զ���������
		static  const char* testCmds2[] = {
			"cd ../user1",
			"ls",
			"open apple2 -rw",//û��Ȩ�ޣ����ܴ�
			"open apple2 -r",//ֻ����
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
	string command = {};//��������
	if (edits.size() == 0) {
		dp.push_back(command);
		return dp;
	}
	for (int i = 0;i < edits.size();i++) {
		if (edits[i] == ' ') {
			if (command.size() != 0) {
				//���Ȳ�Ϊ0
				dp.push_back(command);
				command = "";
			}
		}
		else {
			command = command + edits[i];
		}
	}
	if (command.size() != 0) {
		//���Ȳ�Ϊ0
		dp.push_back(command);
		command = "";
	}
	return dp;
}

//����������û�����
string Operate::User_input() {
	string command;//�û����������
	char coin;//ÿ������ĵ����ַ�
	while (cin.get(coin)) {
		//��������Իس�����
		if (coin == '\n')
			break;
		command = command + coin;
	}
	return command;
}

string Operate::Arg_input(int &autoFlag, User* user) {
	string line = "man";
	textcolor(2);//������ɫ
	cout << "Unix_FileSystem ";
	textcolor(3);//����ǳ��ɫ
	cout << user->curDirPath;
	if (user->pw_name == "root") {
		cout << "$ ";
	}
	else {
		cout << "# ";
	}
	textcolor(7);//���ð�ɫ
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
	cout << "++++++++++++++++++++ Unix�ļ�ϵͳģ�� ++++++++++++++++++++" << endl;
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
		line = Arg_input(autoFlag, user);//��������
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
			//create û����Ҫģʽ�ı�Ҫ
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

//�����ı������ɫ
void Operate::textcolor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
//1	    ����ɫ
//2	    ����ɫ
//3	    ����ɫ
//4	    ���ɫ
//5	    ���ɫ
//6	    ��ɫ
//7	    ���ɫ
//8	    ��ɫ
//9	    ǳ��ɫ
//10	ǳ��ɫ
//11	ǳ��ɫ
//12	ǳ��ɫ
//13	ǳ��ɫ
//14	ǳ��ɫ
//15	ǳ��ɫ
