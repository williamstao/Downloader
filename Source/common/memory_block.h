#ifndef __MEMORY_BLOCK_H__
#define __MEMORY_BLOCK_H__
#include "common.h"

class memory_block
{
public:
	memory_block();
	~memory_block();

public:
	bool read_file(const char *path);
	bool alloc_memory(unsigned int len);
	bool set_binary(const unsigned char *data, unsigned int len);
	unsigned char *get_binary() const;
	unsigned int get_binary_length() const;
	unsigned int get_str_length() const;
	void release();
	bool append_binary(const unsigned char *data, size_t len);
	int dump(const char *path);

private:
	unsigned char *m_binary;
	unsigned int m_length;
	size_t m_capacity;
};

#endif // !__MEMORY_BLOCK_H__
