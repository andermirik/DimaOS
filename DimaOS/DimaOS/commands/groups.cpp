#include "../globals.h"
#include <iostream>
#include "../utils.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;

void set_groups_commands() {
	GV::cmds["groups"] = [](std::vector<string> args) {
		if (!args.empty()) {
			if (args[0] == "add") {
				string name, pswd;
				if (args.size() > 1) {
					name = args[1];
				}
				else {
					cout << "имя группы: ";
					std::getline(cin, name);
				}
				if (name != "default") {
					cout << "пароль: ";
					pswd = util::read_pswd();

					GV::os.sudo();
					int gid = GV::os.group_add(name, pswd);
					GV::os.suend();

					if (gid != -1) {
						cout << "группа " << name << " с gid " << gid << " успешно добавлен." << endl;
					}
					else {
						cout << "во время добавления группы возникла ошибка." << endl;
					}
				}
				else {
					cout << "группу default создать нельзя";
				}
			}
			else if (args[0] == "get") {
				cout << "список групп: " << endl;
				GV::os.sudo();
				for (auto& [gid, oid, name] : GV::os.group_get()) {
					printf("gid: %5d\toid: %5d\tname: %s\n", gid, oid, name.c_str());
				}
				GV::os.suend();
			}
			else if (args[0] == "delete") {
				string name, pswd;
				if (args.size() > 1) {
					name = args[1];
				}
				else {
					cout << "имя группы: ";
					std::getline(cin, name);
				}
				
				cout << "пароль: ";
				pswd = util::read_pswd();

				GV::os.sudo();
				int uid = GV::os.group_del(name, pswd);
				GV::os.suend();

				if (uid != -1) {
					cout << "группа " << name << " успешно удалена!" << endl;
				}
				else {
					cout << "ошибка во время удаления группы" << endl;
				}
			}
			else if (args[0] == "rename") {
				string name, pswd, new_name;
				if (args.size() > 1) {
					name = args[1];
				}
				else {
					cout << "имя группы: ";
					std::getline(cin, name);
				}

				cout << "пароль: ";
				pswd = util::read_pswd();

				cout << "новое имя: ";
				std::getline(cin, new_name);

				GV::os.sudo();
				int err = GV::os.group_rename(name, pswd, new_name);
				GV::os.suend();
				if (err == 0) {
					cout << "группа успешно переименована" << endl;
				}
				else {
					cout << "произошла ошибка" << endl;
				}

			
			}
			else if (args[0] == "passwd") {
				string name, pswd, new_pswd;
				if (args.size() > 1) {
					name = args[1];
				}
				else {
					cout << "имя группы: ";
					std::getline(cin, name);
				}

				cout << "пароль: ";
				pswd = util::read_pswd();

				cout << "новый пароль: ";
				new_pswd = util::read_pswd();

				GV::os.sudo();
				int err = GV::os.group_pswd(name, pswd, new_pswd);
				GV::os.suend();
				if (err == 0) {
					cout << "группа успешно сменила пароль" << endl;
				}
				else {
					cout << "произошла ошибка" << endl;
				}
			}
			else if (args[0] == "leave") {
				cout << "вы точно хотите покинуть группу? (y/else): ";
				string v;
				std::getline(cin, v);
				if (v == "y") {
					GV::os.sudo();
					int err = GV::os.group_leave();
					GV::os.suend();
				}
				
			}
			else if (args[0] == "join") {
				string name, pswd;
				if (args.size() > 1) {
					name = args[1];
				}
				else {
					cout << "имя группы: ";
					std::getline(cin, name);
				}

				cout << "пароль: ";
				pswd = util::read_pswd();

				GV::os.sudo();
				int uid = GV::os.group_join(name, pswd);
				GV::os.suend();
				if (uid == 0) {
					cout << "вы успешно присоеденились к группе " << name << endl;
				}
				else {
					cout << "произошла ошибка" << endl;
				}
			}
		}
		else {
			cout << "add - создать группу" << endl;
			cout << "delete - удалить группу" << endl;
			cout << "rename - переименовать группу" << endl;
			cout << "passwd - сменить пароль группы" << endl;
			cout << "leave - выйти из группы" << endl;
			cout << "join - присоединиться к группе" << endl;
		}
	};
}