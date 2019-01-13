#ifndef __AUTO_RELEASE_POOL__
#define __AUTO_RELEASE_POOL__
#include "common.h"

class MallocReleasePool
{
public:
	virtual ~MallocReleasePool();
	void Append(void *ptr);
private:
	std::vector<void *> m_ptr_list;
};

class FileClosePool
{
public:
	virtual ~FileClosePool();
	void Append(FILE *pfile);
private:
	std::vector<FILE *> m_pfiles;
};

#endif
