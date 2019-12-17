#include "utils.h"
#include "core.h"
#include "os.h"
#include <iostream>
#include <bitset>
#include "windows.h"

#include <map>
#include <functional>

#include <sstream>

using std::string;
using std::cout;
using std::cin;
using std::endl;

std::tuple<string, std::vector<string>> parse_line(string & line) {

	string command = line.substr(0, line.find(' '));
	std::vector<string> args;
	if (command.size() != line.size())
		args = util::split(line.substr(command.size() + 1), ' ');

	return std::make_tuple(command, args);
}

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(0, "rus");

	util::set_text_color(colors::White);	

	set_commands();
	
	/*
	1 - для рестарта системы при запуске
	0 - не рестартить
	*/

#if 1
	GV::cmds["init"]({});//тут удалить
	GV::os.current_user = LazyOS::user();
#endif

	auto root = GV::os.read_inode(0);
	char buf[512];
	char buf2[512];
	GV::os.read_block_indirect(root, 0, buf);
	GV::os.read_block_indirect(root, 1, buf2);

	GV::cmds["users"]({ "login" });
	if (std::string(GV::os.current_user.login) == ""){
		system("pause");
		return 0;
	}

	string line;
	while (true) {
		
		cout << GV::os.current_user.login;
		cout << "$ ";
		std::getline(std::cin, line);

		auto[command, args] = parse_line(line);
		
		bool worked = false;
		for (auto& it : GV::cmds) {
			if (it.first == command) {
				worked = true;
				it.second(args);
				break;
			}
		}
		if (!worked) {
			cout << "Не найдена комманда " << command << endl;
		}

	}

	return(0);
}
