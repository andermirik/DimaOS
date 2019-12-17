#include "../globals.h"
#include <iostream>
#include "../utils.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;

void set_default_commands() {
	GV::cmds["init"] = [](std::vector<string> args) {
		if (!args.empty())
			GV::os.resize(std::stoi(args[0]) << 20, 512);
		else
			GV::os.resize(512 << 20, 512);
	};

	GV::cmds["exit"] = [](std::vector<string> args) {
		exit(0);
	};

	GV::cmds["clear"] = [](std::vector<string> args) {
		system("cls");
	};

	GV::cmds["help"] = [](std::vector<string> args) {
		cout << "��������������� ���� ������ DimaOS" << endl
			<< "groups - ��� ������� ��� ������" << endl
			<< "users - ������� ��� ������������" << endl
			<< "ls - �������������� ���������� ������" << endl
			<< "dir - ����������� ����� ������ " << endl
			<< "mk - ������� ����" << endl
			<< "rm - ������� ����" << endl
			<< "mv - ����������� ����" << endl
			<< "cp - ���������� ����" << endl
			<< "append - �������� � ����� �����" << endl
			<< "read - ��������� � �����" << endl
			<< "write - �������� � ����" << endl
			<< "chmod - ���������� ����� �����" << endl
			<< "add - ������� ������" << endl
		    << "get - �������� ������ �����" << endl
		    << "delete - ������� ������" << endl
		    << "rename - ������������� ������" << endl
		    << "passwd - ������� ������ ������" << endl
		    << "leave - ����� �� ������" << endl
		    << "join - �������������� � ������" << endl
			<< "clear - �������� �������" << endl
		    << "exit - �����" << endl
			<< "asdasdas" << endl;
	};
}