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
	GV::cmds["whereami"] = [](std::vector<string> args) {
		cout << "вы сейчас находитесь в " << util::join(GV::os.dirs, "/") << endl;
	};
}