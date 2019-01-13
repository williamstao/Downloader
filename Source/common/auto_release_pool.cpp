#include "auto_release_pool.h"

MallocReleasePool::~MallocReleasePool()
{
	std::vector<void *>::iterator it;
	for (it = m_ptr_list.begin(); it != m_ptr_list.end(); it++)
	{
		free(*it);
		*it = NULL;
	}
}

void MallocReleasePool::Append(void *ptr)
{
	m_ptr_list.push_back(ptr);
}

FileClosePool::~FileClosePool()
{
	std::vector<FILE *>::iterator it;
	for (it = m_pfiles.begin(); it != m_pfiles.end(); it++)
	{
		fclose(*it);
		*it = NULL;
	}
}

void FileClosePool::Append(FILE *pfile)
{
	if (pfile != NULL)
	{
		m_pfiles.push_back(pfile);
	}
}

