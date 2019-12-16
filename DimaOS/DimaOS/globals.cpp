#include "globals.h"

LazyOS GV::os;
std::map<std::string, std::function<void(std::vector<std::string>)>> GV::cmds;

void set_commands()
{
	set_default_commands();
	set_users_commands();
	set_groups_commands();

	set_filesystem_commands();
}
