#include "../globals.h"
#include <iostream>
#include "../utils.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;

void set_users_commands() {
	GV::cmds["who"] = [](std::vector<string> args) {
		cout << "сейчас вы: " << GV::os.current_user.login << endl;
	};

	GV::cmds["users"] = [](std::vector<string> args) {
		if (!args.empty()) {
			if (args[0] == "add") {
				string login, pswd;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "логин: ";
					std::getline(cin, login);
				}
				cout << "пароль: ";
				pswd = util::read_pswd();

				GV::os.sudo();
				int uid = GV::os.user_add(login, pswd);
				GV::os.suend();
				if (uid != -1) {
					cout << "пользователь " << login << " с uid " << uid << " успешно добавлен." << endl;
				}
				else {
					cout << "во время добавления пользователя возникла ошибка." << endl
						<< "проверьте логин еще раз!" << endl;
				}
			}
			else if (args[0] == "get") {
				cout << "список пользователей: " << endl;
				GV::os.sudo();
				for (auto& [uid, gid, login] : GV::os.user_get()) {
					printf("uid: %5d\tgid: %5d\tlogin: %s\n", uid, gid, login.c_str());
				}
				GV::os.suend();

			}
			else if (args[0] == "delete") {
				string login, pswd;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "логин: ";
					std::getline(cin, login);
				}
				cout << "пароль: ";
				pswd = util::read_pswd();

				GV::os.sudo();
				int uid = GV::os.user_login(login, pswd);
				GV::os.suend();
				if (uid != -1) {
					GV::os.sudo();
					GV::os.user_del(login);
					GV::os.suend();
					cout << "пользователь " << login << " успешно удалён!" << endl;
				}
				else {
					cout << "не верный логин или пароль" << endl;
				}
			}
			else if (args[0] == "rename") {
				string login, pswd, new_login;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "логин: ";
					std::getline(cin, login);
				}
				cout << "пароль: ";
				pswd = util::read_pswd();

				cout << "новый логин: ";
				std::getline(cin, new_login);

				GV::os.sudo();
				int uid = GV::os.user_login(login, pswd);
				GV::os.suend();
				if (uid != -1) {
					GV::os.sudo();
					GV::os.user_rnm(login, new_login);
					GV::os.suend();
					cout << "пользователь " << login << " успешно переименован!" << endl;
				}
				else {
					cout << "не верный логин или пароль" << endl;
				}
			}
			else if (args[0] == "passwd") {
				string login, pswd, new_pswd;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "логин: ";
					std::getline(cin, login);
				}
				cout << "пароль: ";
				pswd = util::read_pswd();

				cout << "новый пароль: ";
				new_pswd = util::read_pswd();

				GV::os.sudo();
				int uid = GV::os.user_login(login, pswd);
				GV::os.suend();
				if (uid != -1) {
					GV::os.sudo();
					GV::os.user_pswd(login, new_pswd);
					GV::os.suend();
					cout << "пользователь " << login << " успешно сменил пароль!" << endl;
				}
				else {
					cout << "не верный логин или пароль" << endl;
				}
			}
			else if (args[0] == "login") {
				string login, pswd;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "логин: ";
					std::getline(cin, login);
				}
				cout << "пароль: ";
				pswd = util::read_pswd();

				GV::os.sudo();
				int uid = GV::os.user_login(login, pswd);
				GV::os.suend();
				if (uid != -1) {
					GV::os.sudo();
					auto [_uid, _gid, _login] = GV::os.user_get()[uid];
					GV::os.suend();
					GV::os.current_user = LazyOS::user(uid, login, pswd);
					GV::os.current_user.gid = _gid;
				}
				else {
					cout << "не верный логин или пароль" << endl;
				}
			}
			else {
				cout << "неизвестный аргумент " << args[0] << endl;
			}
		}
		else {
			//print legend
			cout << "get - получить список пользователей" << endl;
			cout << "add - добавить пользователя" << endl;
			cout << "delete - удалить пользователя" << endl;
			cout << "rename - переименовать пользователя" << endl;
			cout << "passwd - смена пароля пользователя" << endl;
			cout << "login - сменить пользователя" << endl;
		}
	};
}