#include "os.h"
#include "utils.h"
#include <fstream>
#include "core.h"
#include <tuple>
using std::string;


std::string DimaOS::relative_to_full_path(std::string path)
{
	return path;
}

void DimaOS::sudo()
{
	sudo_temp_user = GV::os.current_user;
	GV::os.current_user = DimaOS::user(0, "root", "");
	GV::os.current_user.gid = 0;
}

void DimaOS::suend()
{
	GV::os.current_user = sudo_temp_user;
}

std::vector<std::tuple<uint32_t, uint32_t, std::string>> DimaOS::user_get()
{
	std::vector<std::tuple<uint32_t, uint32_t, std::string>> users;

	int inode_number = core::fopen("users");
	int size = core::fsize(inode_number);

	user temp_user;
	for (int i = 0; i < size / 64; i++) {
		core::fread(inode_number, i * 64, 64, (char*)&temp_user);
		if(std::string(temp_user.login)!="")
			users.push_back(std::make_tuple(temp_user.uid, temp_user.gid, temp_user.login));
	}

	return users;
}
int DimaOS::user_login(std::string login, std::string pswd)
{
	pswd = util::stupid_hash(pswd);

	int inode_number = core::fopen("users");
	int size = core::fsize(inode_number);

	for (int i = 0; i < size / 64; i++) {
		user temp_user;
		core::fread(inode_number, i * 64, 64, (char*)&temp_user);
		if (std::string(temp_user.login) == login && std::string(temp_user.pswd) == pswd) {
			return temp_user.uid;
		}
	}
	return -1;
}
int DimaOS::user_add(std::string login, std::string pswd)
{
	pswd = util::stupid_hash(pswd);
	int inode_number = core::fopen("users");
	int size = core::fsize(inode_number);
	
	for (int i = 0; i < size / 64; i++) {
		user temp_user;
		core::fread(inode_number, i*64, 64, (char*)&temp_user);
		if (strcmp(temp_user.login, login.c_str()) == 0) {
			return -1;
		}
	}
	user write_user(size / 64, login, pswd);
	core::fappend(inode_number, 64, (char*)&write_user);
	//����� ���� �������� �������� ��� ������ ������������
	/*if (size != 0) {
		core::fcreate("/home/" + std::string(login) + "/");
		inode attrs = core::fget_attributes(core::fopen("/home/" + std::string(login) + "/"));
		attrs.uid = size/64;
		attrs.gid = write_user.gid;
		core::fset_attributes(core::fopen("/home/" + std::string(login) + "/"), attrs);
	}*/

	return write_user.uid;
}
int DimaOS::user_del(std::string login)
{
	int inode_number = core::fopen("users");
	int size = core::fsize(inode_number);

	for (int i = 0; i < size / 64; i++) {
		user temp_user;
		core::fread(inode_number, i * 64, 64, (char*)&temp_user);
		if (std::string(temp_user.login) == login) {
			if (i != 0) {
				//strcpy_s(temp_user.login, "");
				temp_user.uid = 0xFFFFFFFF;
				core::fwrite(inode_number, i * 64, 64, (char*)&temp_user);
				return temp_user.uid;
			}
		}
	}

	return -1;
}
int DimaOS::user_rnm(std::string login, std::string new_login)
{
	int inode_number = core::fopen("users");
	int size = core::fsize(inode_number);

	for (int i = 0; i < size / 64; i++) {
		user temp_user;
		core::fread(inode_number, i * 64, 64, (char*)&temp_user);
		if (strcmp(temp_user.login, login.c_str()) == 0) {
			if (i != 0) {
				strcpy_s(temp_user.login, new_login.c_str());
				core::fwrite(inode_number, i * 64, 64, (char*)&temp_user);
				core::frename("/home/" + std::string(login) + "/", new_login);
				if (GV::os.sudo_temp_user.uid == temp_user.uid)
					strcpy(GV::os.sudo_temp_user.login, new_login.c_str());
				return temp_user.uid;
			}
			else {
				return -1;
			}
		}
	}

	return -1;
}
int DimaOS::user_pswd(std::string login, std::string new_pswd)
{
	new_pswd = util::stupid_hash(new_pswd);
	int inode_number = core::fopen("users");
	int size = core::fsize(inode_number);

	for (int i = 0; i < size / 64; i++) {
		user temp_user;
		core::fread(inode_number, i * 64, 64, (char*)&temp_user);
		if (strcmp(temp_user.login, login.c_str()) == 0) {
			
			strcpy_s(temp_user.pswd, new_pswd.c_str());
			core::fwrite(inode_number, i * 64, 64, (char*)&temp_user);
			return temp_user.uid;
			
		}
	}

	return -1;
}

DimaOS::user DimaOS::user_read(int user_number)
{
	user ret;
	int inode_number = core::fopen("users");
	core::fread(inode_number, user_number * 64, 64, (char*)&ret);
	return ret;
}
void DimaOS::user_write(int user_number, user& u)
{
	int inode_number = core::fopen("users");
	core::fwrite(inode_number, user_number * 64, 64, (char*)&u);
}

std::vector<std::tuple<uint32_t, uint32_t, std::string>> DimaOS::group_get()
{
	std::vector<std::tuple<uint32_t, uint32_t, std::string>> users;

	int inode_number = core::fopen("groups");
	int size = core::fsize(inode_number);

	group temp_group;
	for (int i = 0; i < size / 64; i++) {
		core::fread(inode_number, i * 64, 64, (char*)&temp_group);
		users.push_back(std::make_tuple(temp_group.gid, temp_group.oid, temp_group.name));
	}

	return users;
}
int DimaOS::group_add(std::string name, std::string pswd)
{
	pswd = util::stupid_hash(pswd);
	if (sudo_temp_user.gid == 0xFFFFFFFF || sudo_temp_user.uid == 0) {

		int inode_number = core::fopen("groups");
		int size = core::fsize(inode_number);

		for (int i = 0; i < size / 64; i++) {
			if (sudo_temp_user.uid == 0) {
				return -1;
			}
			group temp_group;
			core::fread(inode_number, i * 64, 64, (char*)&temp_group);
			if (strcmp(temp_group.name, name.c_str()) == 0) {
				return -1;
			}
		}
		group write_group(size / 64, sudo_temp_user.uid, name, pswd);
		core::fappend(inode_number, 64, (char*)&write_group);

		sudo_temp_user = user_read(sudo_temp_user.uid);
		sudo_temp_user.gid = write_group.gid;

		user_write(sudo_temp_user.uid, sudo_temp_user);

		return write_group.gid;
	}
	else {
		return -1;
	}
}
int DimaOS::group_del(std::string name, std::string pswd)
{
	pswd = util::stupid_hash(pswd);
	int inode_number = core::fopen("groups");
	int size = core::fsize(inode_number);

	for (int i = 0; i < size / 64; i++) {
		group temp_group;
		core::fread(inode_number, i * 64, 64, (char*)&temp_group);
		if (std::string(temp_group.name) == name && std::string(temp_group.pswd) == pswd) {
			if (temp_group.gid != 0 && temp_group.gid != -1) {
				temp_group.gid = -1;
				core::fwrite(inode_number, i * 64, 64, (char*)&temp_group);
				return 0;
			}
			else {
				return -1;
			}
		}
	}
	return -1;
}

int DimaOS::group_rename(std::string name, std::string pswd, std::string new_name)
{
	pswd = util::stupid_hash(pswd);
	int inode_number = core::fopen("groups");
	int size = core::fsize(inode_number);

	for (int i = 0; i < size / 64; i++) {
		group temp_group;
		core::fread(inode_number, i * 64, 64, (char*)&temp_group);
		if (std::string(temp_group.name) == name && std::string(temp_group.pswd) == pswd) {
			if (temp_group.gid != 0 && temp_group.gid != -1) {
				strcpy(temp_group.name, new_name.c_str());
				core::fwrite(inode_number, i * 64, 64, (char*)&temp_group);
				return 0;
			}
			else {
				return -1;
			}
		}
	}
	return -1;
}

int DimaOS::group_pswd(std::string name, std::string pswd, std::string new_pswd)
{
	pswd = util::stupid_hash(pswd);
	int inode_number = core::fopen("groups");
	int size = core::fsize(inode_number);

	for (int i = 0; i < size / 64; i++) {
		group temp_group;
		core::fread(inode_number, i * 64, 64, (char*)&temp_group);
		if (std::string(temp_group.name) == name && std::string(temp_group.pswd) == pswd) {
			if (temp_group.gid != 0 && temp_group.gid != -1) {
				strcpy(temp_group.pswd, new_pswd.c_str());
				core::fwrite(inode_number, i * 64, 64, (char*)&temp_group);
				return 0;
			}
			else {
				return -1;
			}
		}
	}
	return -1;
}

int DimaOS::group_leave()
{
	if (sudo_temp_user.uid != 0) {
		GV::os.sudo_temp_user.gid = 0xFFFFFFFF;
		user_write(sudo_temp_user.uid, sudo_temp_user);
	}
	return 0;
}

int DimaOS::group_join(std::string name, std::string pswd)
{
	pswd = util::stupid_hash(pswd);
	int inode_number = core::fopen("groups");
	int size = core::fsize(inode_number);

	for (int i = 0; i < size / 64; i++) {
		group temp_group;
		core::fread(inode_number, i * 64, 64, (char*)&temp_group);
		if (std::string(temp_group.name) == name && std::string(temp_group.pswd) == pswd) {
			if (temp_group.gid != -1 && sudo_temp_user.uid !=0) {
				sudo_temp_user.gid = temp_group.gid;
				user_write(sudo_temp_user.uid, sudo_temp_user);
				return 0;
			}
			else {
				return -1;
			}
		}
	}
	return -1;
}



DimaOS::DimaOS()
{
	file.open("filesystem.dat", std::ios::in | std::ios::out | std::ios::binary);
	superblock = read_super_block();
}

DimaOS::~DimaOS()
{
	file.close();
}

int DimaOS::resize(int size, int size_claster)
{
	file.close();
	std::remove("filesystem.dat");
	file.open("filesystem.dat", std::ios::out | std::ios::binary);
	file.seekp(size - 1);
	file.write("", 1);
	file.close();
	file.open("filesystem.dat", std::ios::in | std::ios::out | std::ios::binary);

	super_block sb;
	sb.magic = 0xED0DDCBA;//0xBADC0DED;
	sb.block_size = 512;
	sb.bitmap_size = size / size_claster/8;
	sb.inodes_in_block = 5;
	sb.inode_blocks = 20000;
	sb.root_inode = 0;


	write_super_block(sb);
	
	superblock = read_super_block();

	int i = 0;
	for (i = 0; i < 256 +1; i++) { //+1 ���� ����
		set_bit(i, 0x1);
	}

	current_user = user(0, "root", util::stupid_hash("qwerty"));
	current_user.gid = 0;

	core::fcreate("/");

	core::fcreate("users");
	core::fcreate("groups");
	
	user_add("root", "qwerty");
	group_add("sup", "qwerty");

	core::fcreate("file1");
	core::fcreate("file2");
	core::fcreate("file3");
	core::fcreate("file4");
	core::fcreate("file5");
	core::fcreate("file6");
	core::fcreate("file7");
	//core::fcreate("file8");

	user_add("ghost", "");
	user_add("andermirik", "hello world!");
	user_add("dim14k", "qwerty");

	return size / size_claster/8;
	return -1;
}



void DimaOS::set_bit(int n, byte value, int offset)
{
	if (value == 0)
		set_bit_0(n+offset);
	else 
		set_bit_1(n+offset);
}

void DimaOS::set_bit_1(int bit_number)
{
	static char buf[512];
	bios_read_sector(bit_number >> 12, buf);
	buf[(bit_number & 4095) >> 3] |= 1 << (bit_number & 7);
	bios_write_sector(bit_number >> 12, buf);
}

void DimaOS::set_bit_0(int bit_number)
{
	static char buf[512];
	bios_read_sector(bit_number >> 12, buf);
	buf[(bit_number & 4095) >> 3] &= ~(1 << (bit_number & 7));
	bios_write_sector(bit_number >> 12, buf);
}

int DimaOS::get_bit(int bit_number, int offset)
{
	bit_number += offset;
	static char buf[512];
	bios_read_sector(bit_number >> 12, buf);
	return (buf[(bit_number & 4095) >> 3] >> (bit_number & 7)) & 1;
}

int DimaOS::bios_read_sector(int sector_number, char buf[512])
{
	file.seekg(sector_number << 9);
	file.read(buf, 512);
	return 0;
}

int DimaOS::bios_write_sector(int sector_number, char buf[512])
{
	file.seekp(sector_number << 9);
	file.write(buf, 512);
	return 0;
}

//max = 20000
void DimaOS::write_inode(int inode_number, inode & input)
{
	file.seekp(256 * 512 + inode_number * sizeof(inode));
	file.write((char*)&input, sizeof(inode));
}

DimaOS::inode DimaOS::read_inode(int inode_number)
{
	inode result;
	file.seekg(256 * 512 + inode_number * sizeof(inode));
	file.read((char*)&result, sizeof(inode));
	return result;
}

void DimaOS::write_super_block(super_block & block)
{
	char buf[512];
	bios_read_sector(0, buf);
	memcpy(buf, &block, sizeof(super_block));
	bios_write_sector(0, buf);
}

DimaOS::super_block DimaOS::read_super_block()
{
	super_block result;
	char buf[512];
	bios_read_sector(0, buf);
	memcpy(&result, buf, sizeof(super_block));
	return result;
}

uint32_t DimaOS::get_free_block()
{
	for (int i = 4023; i < superblock.bitmap_size*8; i++) {
		if (get_bit(i) != 1) {
			return i;
		}
	}
	return 0;
}

uint32_t DimaOS::get_free_inode()
{
	for (int i = 256; i < 4006; i++) {
		if (get_bit(i) != 1) {
			return i-256;
		}
	}
	return 0;
}

void DimaOS::read_block_indirect(inode & inode, int block_number, char buf[512])
{   
	//0 indirect [0 - 11]
	if (block_number < 12) {
		if (inode.blocks[block_number] == 0) {
			inode.blocks[block_number] = get_free_block();
			set_bit(inode.blocks[block_number], 0x1);
		}
		bios_read_sector(inode.blocks[block_number], buf);
	}
	//1 indirect [12 - 155]
	else if (block_number >= 12 && block_number <= 155) {
		uint32_t ind[144] = { 0 };
		if (inode.blocks[12] == 0) {
			inode.blocks[12] = get_free_block();
			set_bit(inode.blocks[12], 0x1);
		}
		bios_read_sector(inode.blocks[12], (char*)ind);
		if (ind[block_number - 12] == 0) {
			ind[block_number - 12] = get_free_block();
			set_bit(ind[block_number - 12], 0x1);
		}
		bios_read_sector(ind[block_number - 12], buf);
	}
	//2 indirect [156 - 155+144*144]
	else if (block_number >= 156 && block_number <= 155 + 144 * 144) {
		uint32_t ind[144] = { 0 };
		if (inode.blocks[13] == 0) {
			inode.blocks[13] = get_free_block();
			set_bit(inode.blocks[13], 0x1);
		}
		bios_read_sector(inode.blocks[13], (char*)ind);
		block_number -= 156;
		if (ind[block_number / 144] == 0) {
			ind[block_number / 144] = get_free_block();
			set_bit(ind[block_number / 144], 0x1);
		}
		bios_read_sector(ind[block_number / 144], (char*)ind);
		if (ind[block_number / 144 + block_number % 144]) {
			ind[block_number / 144 + block_number % 144] = get_free_block();
			set_bit(ind[block_number / 144 + block_number % 144], 0x1);
		}
		bios_read_sector(ind[block_number / 144 + block_number % 144], buf);
	}
	//3 indirect [156+144*144 - 155+144*144 + 144*144*144]
	else if (block_number >= 156 + 144 * 144 && block_number <= 155 + 144 * 144 + 144 * 144 * 144) {
		uint32_t ind[144] = { 0 };
		if (inode.blocks[14] == 0) {
			inode.blocks[14] = get_free_block();
			set_bit(inode.blocks[14], 0x1);
		}
		bios_read_sector(inode.blocks[14], (char*)ind);
		block_number -= 156 + 144 * 144;
		if (ind[(block_number) / 144 / 144] == 0) {
			ind[(block_number) / 144 / 144] = get_free_block();
			set_bit(ind[(block_number) / 144 / 144], 0x1);
		}
		bios_read_sector(ind[(block_number) / 144 / 144], (char*)ind);
		if (ind[(block_number) / 144] == 0) {
			ind[(block_number) / 144] = get_free_block();
			set_bit(ind[(block_number) / 144], 0x1);
		}
		bios_read_sector(ind[(block_number) / 144], (char*)ind);
		if (ind[(block_number) / 144 + (block_number) % 144] == 0) {
			ind[(block_number) / 144 + (block_number) % 144] = get_free_block();
			set_bit(ind[(block_number) / 144 + (block_number) % 144], 0x1);
		}
		bios_read_sector(ind[(block_number) / 144 + (block_number) % 144], buf);
	}
}

void DimaOS::write_block_indirect(inode & inode, int block_number, char buf[512])
{
	//0 indirect [0 - 11]
	if (block_number < 12) {
		if (inode.blocks[block_number] == 0) {
			inode.blocks[block_number] = get_free_block();
			set_bit(256 + inode.blocks[block_number], 0x1);
		}
		bios_write_sector(inode.blocks[block_number], buf);
	}
	//1 indirect [12 - 155]
	else if (block_number >= 12 && block_number <= 155) {
		uint32_t ind[144] = { 0 };
		if (inode.blocks[12] == 0) {
			inode.blocks[12] = get_free_block();
			set_bit(256 + inode.blocks[12], 0x1);
		}
		bios_read_sector(inode.blocks[12], (char*)ind);
		if (ind[block_number - 12] == 0) {
			ind[block_number - 12] = get_free_block();
			set_bit(256 + ind[block_number - 12], 0x1);
		}
		bios_write_sector(ind[block_number - 12], buf);
	}
	//2 indirect [156 - 155+144*144]
	else if (block_number >= 156 && block_number <= 155 + 144 * 144) {
		uint32_t ind[144] = { 0 };
		if (inode.blocks[13] == 0) {
			inode.blocks[13] = get_free_block();
			set_bit(256 + inode.blocks[13], 0x1);
		}
		bios_read_sector(inode.blocks[13], (char*)ind);
		block_number -= 156;
		if (ind[block_number / 144] == 0) {
			ind[block_number / 144] = get_free_block();
			set_bit(256 + ind[block_number / 144], 0x1);
		}
		bios_read_sector(ind[block_number / 144], (char*)ind);
		if (ind[block_number / 144 + block_number % 144]) {
			ind[block_number / 144 + block_number % 144] = get_free_block();
			set_bit(256 + ind[block_number / 144 + block_number % 144], 0x1);
		}
		bios_write_sector(ind[block_number / 144 + block_number % 144], buf);
	}
	//3 indirect [156+144*144 - 155+144*144 + 144*144*144]
	else if (block_number >= 156 + 144 * 144 && block_number <= 155 + 144 * 144 + 144 * 144 * 144) {
		uint32_t ind[144] = { 0 };
		if (inode.blocks[14] == 0) {
			inode.blocks[14] = get_free_block();
			set_bit(256 + inode.blocks[14], 0x1);
		}
		bios_read_sector(inode.blocks[14], (char*)ind);
		block_number -= 156 + 144 * 144;
		if (ind[(block_number) / 144 / 144] == 0) {
			ind[(block_number) / 144 / 144] = get_free_block();
			set_bit(256 + ind[(block_number) / 144 / 144], 0x1);
		}
		bios_read_sector(ind[(block_number) / 144 / 144], (char*)ind);
		if (ind[(block_number) / 144] == 0) {
			ind[(block_number) / 144] = get_free_block();
			set_bit(256 + ind[(block_number) / 144], 0x1);
		}
		bios_read_sector(ind[(block_number) / 144], (char*)ind);
		if (ind[(block_number) / 144 + (block_number) % 144] == 0) {
			ind[(block_number) / 144 + (block_number) % 144] = get_free_block();
			set_bit(256 + ind[(block_number) / 144 + (block_number) % 144], 0x1);
		}
		bios_write_sector(ind[(block_number) / 144 + (block_number) % 144], buf);
	}
}

void DimaOS::lock_inode(int inode_number)
{
	set_bit(256 + inode_number, 0x1);
}

void DimaOS::unlock_inode(int inode_number)
{
	set_bit(256 + inode_number, 0x0);
}

void DimaOS::free_inode_blocks(int inode_number)
{
	auto inode = read_inode(inode_number);
	for (int i = 0; i < 12; i++) {
		inode.blocks[i] = 0;
		set_bit(inode.blocks[i], 0x0);
	}
	//?
	write_inode(inode_number, inode);
}
