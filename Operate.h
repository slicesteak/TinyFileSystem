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

//������
class Operate {
private:
	vector<string> command;//�ʷ�����������

public:

	void man(string command);//��ʾ��ʾ����

	bool autoTest(string& cmd, User* user);//�Զ�������������

	void display();//�û�����

	vector<string> analyse(string edits);//�����������������

	void textcolor(int color);//�����ı������ɫ

	string User_input();//����������û�����

	string Arg_input(int &autoFlag, User* user);//�û������������
};