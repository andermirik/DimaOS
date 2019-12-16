#pragma once
#include <string>
#include "globals.h"

namespace core {
	int fcreate(std::string filename);                       //create file
	int fopen(std::string filename);                         //open file
	int fdelete(std::string filename);                       //delete file

	int fread(int inode_number, int offset, int size, char* to_buf);    //read file
	int fwrite(int inode_number, int offset, int size, char*by_buf);    //write file
	int fappend(int inode_number, int buf_size, char* buf_append);      //append file

	uint64_t fsize(int inode_number);                                            
	
	LazyOS::inode fget_attributes(int inode_number);
	int fset_attributes(int inode_number, LazyOS::inode& inode);

	int frename(std::string filename, std::string new_filename);
}