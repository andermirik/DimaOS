#include "../globals.h"
#include "../core.h"
#include "../utils.h"
#include <iostream>
#include <bitset>
#include <chrono>
#include <sstream>
#include <iomanip>
using std::string;
using std::cout;
using std::cin;
using std::endl;



std::string long_to_time(uint64_t ltime) {
	time_t ttime = (time_t)ltime/1000;
	return std::string(ctime(&ttime));
}

void set_filesystem_commands() {
	GV::cmds["ls"] = [](std::vector<std::string> args) {
		if (args.empty()) {
			args.push_back(util::join(GV::os.dirs, "/"));
		}
		int file_inode = core::fopen(args[0]);
		if (file_inode == 0 && args[0] != "/") {
			cout << "такого файла не существует!" << endl;
			return;
		}
		int size = core::fsize(file_inode);
		char* buf = new char[size];
		int bytes_readed = core::fread(file_inode, 0, size, buf);
		if (bytes_readed >= 0) {

			for (int i = 0; i < size / 64; i++) {
				LazyOS::directory_file file;
				memcpy(&file, buf + i * 64, 64);

				LazyOS::inode inode = GV::os.read_inode(file.n_inode);
				auto attrs = core::fget_attributes(file.n_inode);
				uint16_t type = util::read_first_4_bits(attrs.mode);
				uint16_t rules = util::read_rwxrwxrwx(attrs.mode);

				if (type == 0x5D || type == 0xD) {
					util::set_text_color(colors::LightBlue);
					cout << std::setw(15) << std::left
						<< util::file_to_filename(file) + "";
					util::set_text_color(colors::White);
				}
				else {
					cout << std::setw(15) << std::left
						<< util::file_to_filename(file);
				}

				cout << " ";


				std::bitset<9> rwx(rules);
				for (int i = 8; i >= 0; i--) {

					if (rwx[i] == 1) {
						if (i % 3 == 0)
							cout << "x";
						else if (i % 3 == 1)
							cout << "w";
						else if (i % 3 == 2)
							cout << "r";
					}
					else {
						cout << "-";
					}
				}
				cout << "\t";
				cout << " size: " << inode.size << "\t  ";
				cout << " uid: " << inode.uid << "\t  ";
				if (inode.gid == 0xFFFFFFFF)
					cout << " gid: " << "-1" << "\t  ";
				else
					cout << " gid: " << inode.gid << "\t  ";
				cout << " iid: " << file.n_inode << "\t  ";
				cout << util::replace_all(long_to_time(inode.date_modification), "\n", "") << endl;

			}
		}
		else {
			cout << "недостаточно прав!" << endl;
		}
	};
	GV::cmds["dir"] = [](std::vector<std::string> args) {
		if (args.empty()) {
			args.push_back(util::join(GV::os.dirs, "/"));
		}
		int file_inode = core::fopen(args[0]);
		if (file_inode == 0 && args[0] != "/") {
			cout << "такого файла не существует!" << endl;
			return;
		}
		int size = core::fsize(file_inode);
		char* buf = new char[size];

		int bytes_readed = core::fread(file_inode, 0, size, buf);
		if (bytes_readed >= 0) {

			for (int i = 0; i < size / 64; i++) {
				LazyOS::directory_file file;
				memcpy(&file, buf + i * 64, 64);

				auto attrs = core::fget_attributes(file.n_inode);
				uint16_t type = util::read_first_4_bits(attrs.mode);

				if (type == 0x5D || type == 0xD) {
					util::set_text_color(colors::LightBlue);
					cout << util::file_to_filename(file) + "";
					util::set_text_color(colors::White);
				}
				else {
					cout << util::file_to_filename(file);
				}
				cout << " ";
			}
			cout << endl;
		}
		else {
			cout << "недостаточно прав!" << endl;
		}
	};
	GV::cmds["cd"] = [](std::vector<std::string> args) {
		if (!args.empty()) {
			if (args[0] == "") {
				return;
			}
			
			string path = GV::os.relative_to_full_path(args[0]);

			int file_inode = core::fopen(path);
			auto attrs = core::fget_attributes(file_inode);
			int file_type = util::read_first_4_bits(attrs.mode);
			bool is_dir = file_type == 0xD || file_type == 0x5D;
			
			if ((file_inode || path == "/") && is_dir == true) {
				GV::os.dirs = util::split(path, '/');
				if (file_inode != 0) {
					if (path[path.size()-1] != '/') {
						GV::os.dirs.push_back("");
					}
				}
			}
			else {
				cout << "не удалось открыть файл " << path << endl;
			}
		}
	};
	GV::cmds["mk"] = [](std::vector<std::string> args) {
		if (!args.empty()) {
			string path = GV::os.relative_to_full_path(args[0]);
			int err = core::fcreate(path);
			if (err == 0) {
				cout << "не удалось создать файл " << path << endl;
			}
			else if (err == -1) {
				cout << "не удалось создать файл " << path << endl;
				cout << "не достаточно прав доступа." << endl;
			}
		}
	};
	GV::cmds["rm"] = [](std::vector<std::string> args) {
		if (!args.empty()) {
			string path = GV::os.relative_to_full_path(args[0]);
			int err = core::fdelete(path);
			if (err == 0) {
				cout << "не удалось удалить файл " << path << endl;
			}
			else if (err == -1) {
				cout << "не удалось удалить файл " << path << endl;
				cout << "не достаточно прав доступа." << endl;
			}
		}
	};
	GV::cmds["mv"] = [](std::vector <std::string> args) {
		if (args.size() > 1) {
			string path = GV::os.relative_to_full_path(args[0]);
			string path2 = GV::os.relative_to_full_path(args[1]);
			if (path == path2) {
				cout << path2 << " уже существует" << endl;
				return;
			}
			if (path2[path2.size() - 1] == '/') {
				cout << "второй файл не должен быть директорией." << endl;
				return;
			}

			int file_inode = core::fopen(path);
			if (path != "/" && file_inode > 0) {
				int size = core::fsize(file_inode);
				if (util::read_first_4_bits(core::fget_attributes(file_inode).mode) == 0xF) {
					char* buf;
					buf = new char[1];//чтоб не было ошибки
					if (size != 0) {
						buf = new char[size];
						core::fread(file_inode, 0, size, buf);
					}
					if (core::fopen(path2) == 0 && path2 != "/") {
						core::fcreate(path2);
						int new_file_inode = core::fopen(path2);
						if (new_file_inode >= 0 && path2 != "/") {
							if (size != 0) {
								core::fwrite(new_file_inode, 0, size, buf);
							}
						}
						else {
							cout << "не удалось содать файл " << path2 << endl;
						}
						core::fdelete(path);
					}
					else {
						cout << "файл " << path2 << " уже существует" << endl;
					}
				}
				else {
					cout << "ошибка. файл " << path << " является директорией" << endl;
				}
			}
			else {
				cout << "не удалось открыть файл " << path << endl;
			}
		}
	};
	GV::cmds["cp"] = [](std::vector <std::string> args) {
		if (args.size() > 1) {
			string path = GV::os.relative_to_full_path(args[0]);
			string path2 = GV::os.relative_to_full_path(args[1]);
			if (path == path2) {
				cout << path2 << " уже существует" << endl;
				return;
			}
			if (path2[path2.size() - 1] == '/') {
				cout << "второй файл не должен быть директорией." << endl;
				return;
			}

			int file_inode = core::fopen(path);
			if (path != "/" && file_inode > 0) {
				int size = core::fsize(file_inode);
				if (util::read_first_4_bits(core::fget_attributes(file_inode).mode) == 0xF) {
					char* buf;
					buf = new char[1];//чтоб не было ошибки
					if (size != 0) {
						buf = new char[size];
						core::fread(file_inode, 0, size, buf);
					}
					if (core::fopen(path2) == 0 && path2 != "/") {
						core::fcreate(path2);
						int new_file_inode = core::fopen(path2);
						if (new_file_inode >= 0 && path2 != "/") {
							if (size != 0) {
								core::fwrite(new_file_inode, 0, size, buf);
							}
						}
						else {
							cout << "не удалось содать файл " << path2 << endl;
						}
					}
					else {
						cout << "файл " << path2 << " уже существует" << endl;
					}
				}
				else {
					cout << "ошибка. файл " << path << " является директорией" << endl;
				}
			}
			else {
				cout << "не удалось открыть файл " << path << endl;
			}
		}
	};

	GV::cmds["append"] = [](std::vector <std::string> args) {
		if (args.size()>1) {
			string path = GV::os.relative_to_full_path(args[0]);
			int size = std::stoi(args[1]);

			int file_inode = core::fopen(path);
			if (file_inode) {
				char*buf = new char[size];
				cout << "> ";
				cin.read(buf, size);
				if (std::cin.peek()) std::cin.ignore();

				int count_bytes = core::fappend(file_inode, size, buf);
				if (count_bytes >= 0) {
					cout << "дозаписано: " << count_bytes << " байт" << endl;
				}
				else {
					cout << "недостаточно прав!" << endl;
				}
			}
			else {
				cout << "файл " << path << " не удалось открыть." << endl;
			}
		}
		else {
			cout << "недостаточно аргументов. \nструктрура комманды: append <path> <count>" << endl;
		}
	};
	GV::cmds["read"] = [](std::vector <std::string> args) {
		if (args.size() > 2) {
			string path = GV::os.relative_to_full_path(args[0]);
			int offset = std::stoi(args[1]);
			int size = std::stoi(args[2]);

			int file_inode = core::fopen(path);
			if (file_inode) {
				char*buf = new char[size];
				memset(buf, 0, size);
				if (core::fread(file_inode, offset, size, buf) >= 0) {
					cout << "прочитано: " << std::string(buf, size) << endl;
				}
				else {
					cout << "недостаточно прав!" << endl;
				}
			}
			else {
				cout << "файл " << path << " не удалось открыть." << endl;
			}
		}
		else {
			cout << "недостаточно аргументов. \nструктрура комманды: read <path> <offset> <count>" << endl;
		}
	};
	GV::cmds["write"] = [](std::vector <std::string> args) {
		if (args.size() > 2) {
			string path = GV::os.relative_to_full_path(args[0]);
			int offset = std::stoi(args[1]);
			int size = std::stoi(args[2]);

			int file_inode = core::fopen(path);
			if (file_inode) {
				char*buf = new char[size];
				cout << "> ";
				cin.read(buf, size);
				if (std::cin.peek()) std::cin.ignore();
				int count_bytes = core::fwrite(file_inode, offset, size, buf);
				if (count_bytes >= 0) {
					cout << "записано: " << count_bytes << " байт" << endl;
				}
				else {
					cout << "недостаточно прав!" << endl;
				}
			}
			else {
				cout << "файл " << path << " не удалось открыть." << endl;
			}
		}
		else {
			cout << "недостаточно аргументов. \nструктрура комманды: write <path> <offset> <count>" << endl;
		}
	};
	GV::cmds["chmod"] = [](std::vector<std::string> args) {
		if (args.size() > 1) {
			string path = GV::os.relative_to_full_path(args[0]);
			int rwx = std::stoi(args[1], 0, 8);

			int file_inode = core::fopen(path);
			if (file_inode || path == "/") {
				auto attrs = core::fget_attributes(file_inode);
				if (
					GV::os.current_user.uid == 0
					|| GV::os.current_user.gid == 0
					|| attrs.uid == GV::os.current_user.uid 
					|| (attrs.gid == GV::os.current_user.gid && attrs.gid != 0xFFFFFFFF)
					) {
					attrs.mode = util::write_rwxrwxrwx(attrs.mode, rwx);
					core::fset_attributes(file_inode, attrs);
				}
				else {
					cout << "права доступа не изменены." << endl;
					cout << "вы не владелец файла." << endl;
				}
			}
			else {
				cout << "не удалось найти файл " << "path" << endl;
 			}
		}
		else {
			cout << "недостаточно аргументов. chmod <path> <777>" << endl;
		}
	};
}