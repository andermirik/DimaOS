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
		cout << "Вспомогательное меню помощи DimaOS" << endl
			<< "groups - все команды для группы" << endl
			<< "users - команды для пользователя" << endl
			<< "ls - дополнительная информация файлов" << endl
			<< "dir - просмотреть имена файлов " << endl
			<< "mk - создать файл" << endl
			<< "rm - удалить файл" << endl
			<< "mv - переместить файл" << endl
			<< "cp - копировать файл" << endl
			<< "append - добавить в конец файла" << endl
			<< "read - прочитать в файле" << endl
			<< "write - записать в файл" << endl
			<< "chmod - установить права файла" << endl
			<< "add - создать группу" << endl
		    << "get - получить список групп" << endl
		    << "delete - удалить группу" << endl
		    << "rename - переименовать группу" << endl
		    << "passwd - сменить пароль группы" << endl
		    << "leave - выйти из группы" << endl
		    << "join - присоединиться к группе" << endl
			<< "clear - очистить консоль" << endl
		    << "exit - выйти" << endl
			<< "asdasdas" << endl;
	};
}