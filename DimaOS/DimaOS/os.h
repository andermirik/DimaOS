#pragma once
#define MB 1024*1024
using byte = unsigned char;
#include <fstream>
#include <vector>

class LazyOS {
public:
	struct inode;
	struct super_block;
	struct directory_file;
	struct user;
private:
	std::fstream file;
public:
	
	std::vector<std::string> dirs;

	std::string relative_to_full_path(std::string path);
	void sudo();
	void suend();


	struct group {
		uint32_t gid;
		uint32_t oid;//owner id
		char name[28];
		char pswd[28];
		group() {
			gid = 0;
			oid = 0;
			char name[28] = { 0 };
			char pswd[28] = { 0 };
			memcpy(this->name, name, 28);
			memcpy(this->pswd, pswd, 28);

		};
		group(uint32_t gid, uint32_t oid, std::string name, std::string pswd) {
			strcpy_s(this->name, name.c_str());
			strcpy_s(this->pswd, pswd.c_str());

			this->gid = gid;
			this->oid = oid;
		}
	};

	struct user {
		uint32_t uid;
		uint32_t gid;
		char login[28];
		char pswd[28];
		user() {
			uid = 0;
			gid = 0;
			char login[28] = { 0 };
			char pswd[28] = { 0 };
			memcpy(this->login, login, 28);
			memcpy(this->pswd, pswd, 28);
		};
		user(uint32_t uid, std::string login, std::string pswd) {
			strcpy_s(this->login, login.c_str());
			strcpy_s(this->pswd, pswd.c_str());
			this->uid = uid;
			this->gid = 0xFFFFFFFF;
		};
	};
	user current_user;
	user sudo_temp_user;
	std::vector<std::tuple<uint32_t, uint32_t, std::string>> user_get();

	int user_login(std::string login, std::string pswd);
	int user_add(std::string login, std::string pswd);
	int user_del(std::string login);
	int user_rnm(std::string login, std::string new_login);
	int user_pswd(std::string login, std::string new_pswd);

	user user_read(int user_number);
	void user_write(int user_number, user& u);

	std::vector<std::tuple<uint32_t, uint32_t, std::string>> group_get();

	int group_add(std::string name, std::string pswd);
	int group_del(std::string name, std::string pswd);
	int group_rename(std::string name, std::string pswd, std::string new_name);
	int group_pswd(std::string name, std::string pswd, std::string new_pswd);
	int group_leave();
	int group_join(std::string name, std::string pswd);
	

	LazyOS();
	~LazyOS();
	int resize(int size, int size_claster);

	void set_bit_0(int bit_number);
	void set_bit_1(int bit_number);

	void set_bit(int bit_number, byte value = 1, int offset = 4096);
	int get_bit(int bit_number, int offset = 4096);

	int bios_read_sector(int sector_number, char buf[512]);
	int bios_write_sector(int sector_number, char buf[512]);

	void write_inode(int inode_number, inode& inode);
	inode read_inode(int inode_number);

	void write_super_block(super_block& inode);
	super_block read_super_block();

	uint32_t get_free_block();
	uint32_t get_free_inode();

	void read_block_indirect(inode& inode, int block_number, char buf[512]);
	void write_block_indirect(inode& inode, int block_number, char buf[512]);

	void lock_inode(int inode_number);
	void unlock_inode(int inode_number);

	void free_inode_blocks(int inode_number);


	struct super_block {
		uint32_t magic;            //является ли LazyOS
		uint32_t block_size;       //размер блока
		uint32_t root_inode;       //инод рута
		uint32_t inode_blocks;     //количество инодов
		uint32_t bitmap_size;      //размер bitmap в байтах
		uint32_t inodes_in_block;
	};

	super_block superblock;

	struct inode {
		                     
		/* 
		[
			|type|     тип файла. 4 бита
			|u|        suid 
			|g|        sgid 
			|s|        sticky
			|r|w|x|    владелец
			|r|w|x|    группа
			|r|w|x|    остальные
		]
		*/
		uint16_t mode;
		uint16_t nlinks;
		uint32_t uid;
		uint32_t gid;
		uint32_t blocks[15];
		uint64_t size;
		uint64_t date_creation;
		uint64_t date_modification;
	};

	struct directory_file {
		uint32_t n_inode;
		char filename[54];
		char extension[6];

		directory_file() {
			this->n_inode = 0;
			char filename[54] = { 0 };
			char extension[6] = { 0 };
			memcpy(this->filename, filename,  54);
			memcpy(this->extension, extension, 6);
		};

		directory_file(uint32_t n_inode, std::string filename) {
			this->n_inode = n_inode;

			char f[54] = { 0 };
			char e[6] = { 0 };

			memcpy(this->filename, f, 54);
			memcpy(this->extension, e, 6);

			if(filename.find_last_of('.') != std::string::npos)
				memcpy(this->filename,
					filename.substr(0, filename.find_last_of('.')).c_str(),
					filename.substr(0, filename.find_last_of('.')).size()
				);
			else 
				memcpy(this->filename,
					filename.c_str(),
					filename.size()
				);
			if (filename.find_last_of('.') != std::string::npos)
				memcpy(this->extension,
					filename.substr(filename.find_last_of('.')).c_str(),
					filename.substr(filename.find_last_of('.')).size()
				);
		}
	};

};

/*
sb: [0;0]
bm: [1;256]
id: [257;4006]
/:  [4007, 4022]
*/