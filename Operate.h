#pragma once
#include "BufferManager.h"
#include "OpenFileManager.h"
#include "FileManager.h"
#include "User.h"
#include<iostream>
#include<vector>
#include<string>
#include<windows.h>
using namespace std;

//操作类
class Operate {
private:
	vector<string> command;//词法分析的命令

public:

	void man(string command);//显示提示内容

	bool autoTest(string& cmd, User* user);//自动测试命令输入

	void display();//用户操作

	vector<string> analyse(string edits);//对输入的命令做分析

	void textcolor(int color);//设置文本输出颜色

	string User_input();//经过处理的用户输入

	string Arg_input(int &autoFlag, User* user);//用户命令输入操作
};