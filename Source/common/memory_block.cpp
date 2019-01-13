#include "memory_block.h"
#include "file_helper.h"

memory_block::memory_block() :m_binary(NULL), m_length(0), m_capacity(0)
{

}

memory_block::~memory_block()
{
	release();
}

void memory_block::release()
{
	if (m_binary)
	{
		free(m_binary);
		m_binary = NULL;
	}
	m_length = 0;
	m_capacity = 0;
}

bool memory_block::read_file(const char *path)
{
	release();

	struct _stat st = { 0 };
	if (_stat(path, &st) != 0)
	{
		return false;
	}
	FILE *f = fopen(path, "rb");
	if (f == NULL)
	{
		return false;
	}
	if (!alloc_memory((unsigned int)st.st_size))
	{
		fclose(f);
		return false;
	}
	size_t cnt = fread(m_binary, m_length, 1, f);
	if (cnt != 1)
	{
		fclose(f);
		release();
		return false;
	}

	fclose(f);
	return true;
}
bool memory_block::alloc_memory(unsigned int len)
{
	release();
	m_binary = (unsigned char *)malloc(len + 1);
	if (m_binary == NULL)
	{
		return false;
	}
	m_length = len;
	m_binary[len] = '\0';
	return true;
}
bool memory_block::set_binary(const unsigned char *data, unsigned int len)
{
	if (!alloc_memory(len))
	{
		return false;
	}
	memcpy(m_binary, data, len);
	m_length = len;

	return true;
}

bool memory_block::append_binary(const unsigned char *data, size_t len)
{
	if (len < 1)
	{
		return false;
	}

	//判断内存是否够用
	if (m_length + len <= m_capacity)
	{
		memcpy(m_binary + m_length, data, len);
		m_length += len;
		m_binary[m_length] = 0;
		return true;
	}
	//不够用的话,开辟新内存
	size_t capacity = m_length + len;
	capacity *= 2;

	//开辟内存
	unsigned char *ptr = (unsigned char *)realloc(m_binary, capacity + 1);
	if (ptr == NULL)
	{
		__ASSERT(0);
		return false;
	}
	m_binary = ptr;
	m_capacity = capacity;
	memcpy(m_binary + m_length, data, len);
	m_length += len;
	m_binary[m_length] = 0;
	return true;
}

unsigned char *memory_block::get_binary() const
{
	return m_binary;
}

unsigned int memory_block::get_binary_length() const
{
	return m_length;
}

unsigned int memory_block::get_str_length() const
{
	if (m_length < 1)
	{
		return 0;
	}
	return m_length - 1;
}

int memory_block::dump(const char *path)
{
	return tp_dump_file(path, m_binary, m_length);
}
