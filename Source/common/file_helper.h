#ifndef __FILE_HELPER_H_
#define __FILE_HELPER_H 
#include "common.h"

enum file_tag
{
	e_drive = 1,
	e_dir = 2,
	e_fname = 3,
	e_ext = 4
};

int tp_copy_file(const char* src_path, const char* tar_path);
int tp_dump_file(const char* path, const unsigned char* buf, size_t buf_len);
	
int tp_get_folder(const std::string& path, std::string& folder);
int tp_get_file_size(const std::string& path, uint32_t& file_size);
int tp_get_file_name(const std::string& path, std::string& file_name);
int tp_get_file_basename(const std::string& path, std::string& file_basename);
int tp_get_file_ext(const std::string& path, std::string& file_ext);
	
int tp_is_file_exists_by_md5(char* folder, const char* md5);
#endif