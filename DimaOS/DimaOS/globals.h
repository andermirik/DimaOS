#pragma once
#include "os.h"
#include <map>
#include <functional>
#include <string>

void set_default_commands();
void set_users_commands();
void set_groups_commands();
void set_filesystem_commands();

void set_commands();

class GV {
public:
	static LazyOS os;
	static std::map<std::string, std::function<void(std::vector<std::string>)>> cmds;
};
