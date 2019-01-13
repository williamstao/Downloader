#include "file_helper.h"
#include "auto_release_pool.h"
#include "md5.h"

int tp_copy_file(const char* src_path, const char* tar_path)
{
	if (src_path == NULL || tar_path == NULL)
	{
		__ASSERT(0);
		return -1;
	}
	if (strcmp(src_path, tar_path) == 0)
	{
		return 0;
	}
	FileClosePool pool;

	FILE *src_f = NULL;
	FILE *tar_f = NULL;

	src_f = fopen(src_path, "rb");
	if (src_f == NULL)
	{
		__DEBUG_LOG("open %s for read err, errno:%d:%s", src_path, errno, strerror(errno));
		return -1;
	}
	pool.Append(src_f);

	tar_f = fopen(tar_path, "wb");
	if (tar_f == NULL)
	{
		__ASSERT(0);
		return -1;
	}
	pool.Append(tar_f);
	unsigned char buf[4096];
	size_t retval_1, retval_2;
	while (1)
	{
		retval_1 = fread(buf, 1, sizeof(buf), src_f);
		if (retval_1 > 0 && retval_1 <= sizeof(buf))
		{
			retval_2 = fwrite(buf, 1, retval_1, tar_f);
			if (retval_2 != retval_1)
			{
				__ASSERT(0);
				return -1;
			}
		}
		if (retval_1 != sizeof(buf) && ferror(src_f))
		{
			return -1;
		}
		if (feof(src_f)) break;
	}
	return 0;
}

int tp_dump_file(const char* path, const unsigned char* buf, size_t buf_size)
{
	if (path == NULL) return -1;
	if (buf == NULL) return -1;
	if (strlen(path) == 0) return -1;
	if (buf_size < 1) return -1;

	std::string tmp_path(path);
	tmp_path.append(".tmp");

	FILE *f = fopen(tmp_path.c_str(), "wb");
	if (f == NULL)
	{
		return -1;
	}

	size_t off = 0;
	size_t block_size = 0;
	size_t retval = 0;
	size_t max_block_size = 4096;
	while (off < buf_size)
	{
		block_size = buf_size - off;
		if (block_size > max_block_size)
		{
			block_size = max_block_size;
		}
		retval = fwrite(buf + off, 1, block_size, f);
		if (retval != block_size)
		{
			__ASSERT(0);
			fclose(f);
			unlink(tmp_path.c_str());
			return -1;
		}
		off += max_block_size;
	}
	fclose(f);

	unlink(path);
	int err = rename(tmp_path.c_str(), path);
	if (err != 0)
	{
		__DEBUG_LOG("rename %s to %s err, errno:%d:%s", tmp_path.c_str(), path, errno, strerror(errno));
	}
	__ASSERT_EQUAL(err, 0);
	__ASSERT(access(tmp_path.c_str(), 0) != 0);
	__ASSERT(access(path, 0) == 0);

	if (access(path, 0) == 0)
	{
		return 0;
	}

	return -1;
}

static int FileHelperWrapperSplitePath(const std::string& in, int type, std::string& out)
{
	const char* path = in.c_str();
	if (path == NULL || path[0] != 0)
	{
		return -1;
	}

	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char fname[_MAX_FNAME] = { 0 };
	char ext[_MAX_EXT] = { 0 };

	_splitpath(path, drive, dir, fname, ext);
	if (dir[strlen(dir) - 1] == '\\' || dir[strlen(dir) - 1] == '/')
	{
		dir[strlen(dir) - 1] = '\0';
	}

	switch (type)
	{
	case e_drive:
		out = std::string(drive);
		break;
	case e_dir:
		out = std::string(dir);
		break;
	case e_fname:
		out = std::string(fname);
		break;
	case e_ext:
		out = std::string(ext);
		break;
	default:
		break;
	}
	return 0;
}


int tp_get_folder(const std::string& path, std::string& folder)
{
	std::string drive = "";
	int err = FileHelperWrapperSplitePath(path, e_drive, drive);
	if (err != 0)
	{
		return -1;
	}

	std::string dir = "";
	err = FileHelperWrapperSplitePath(path, e_dir, dir);
	if (err != 0)
	{
		return -1;
	}

	folder = drive;
	folder += dir;

	return 0;
}

int tp_get_file_basename(const std::string& path, std::string& file_basename)
{
	std::string fname = "";
	int err = FileHelperWrapperSplitePath(path, e_fname, fname);
	if (err != 0)
	{
		return -1;
	}

	file_basename = fname;
	return 0;
}

int tp_get_file_name(const std::string& path, std::string& file_name)
{
	std::string basename = "";
	int err = FileHelperWrapperSplitePath(path, e_fname, basename);
	if (err != 0)
	{
		return -1;
	}

	std::string ext = "";
	err = FileHelperWrapperSplitePath(path, e_ext, ext);
	if (err != 0)
	{
		return -1;
	}
	basename.append(ext);

	return 0;
}

int tp_get_file_ext(const std::string& path, std::string& file_ext)
{
	int err = FileHelperWrapperSplitePath(path, e_ext, file_ext);
	if (err != 0)
	{
		return -1;
	}
	return 0;
}

int tp_get_file_size(const std::string& path, uint32_t& file_size)
{
	const char* in_path = path.c_str();
	if (in_path == NULL || in_path[0]==0) return -1;

	struct stat st = { 0 };
	if (stat(in_path, &st) != 0) return -1;
	file_size = st.st_size;
	return 0;
}

int tp_is_file_exists_by_md5(char* folder, const char* md5)
{
	if (folder == NULL || folder[0] == 0 || md5 == NULL || md5[0] == 0)
	{
		__ASSERT(0);
		return -1;
	}

	if (access(folder, 0) != 0)
	{
		__ASSERT(0);
		return -1;
	}

	char sxFind[MAX_PATH] = { 0 };
	WIN32_FIND_DATA FindFileData;

	strcpy(sxFind, folder);
	strcat(sxFind, "\\*.*");

	HANDLE hFind = ::FindFirstFile(sxFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind) return 0;

	do {
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
		}
		else
		{
			char file_path[MAX_PATH] = { 0 };
			char file_md5[MAX_MD5_LEN] = { 0 };
			strcpy(file_path, folder);
			strcat(file_path, "\\");
			strcat(file_path, (char*)FindFileData.cFileName);
			int ret = GetFileMd5(file_path, file_md5);
			if (ret == -1)
			{
				__ASSERT(0);
				FindClose(hFind);
				return 0;
			}

			if (strncmp(md5, file_md5, strlen(md5)) == 0)
			{
				FindClose(hFind);
				return 1;
			}
		}
	} while (FindNextFile(hFind, &FindFileData) != 0);

	FindClose(hFind);
	return 0;
}

