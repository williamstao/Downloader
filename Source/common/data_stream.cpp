#include "data_stream.h"
void gt_decrypt_string(unsigned char *buf, size_t buf_size)
{
	const char* key = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (size_t i = 0; i < buf_size; i++)
	{
		buf[i] ^= key[i % 36];
	}
}

DataStream::DataStream(unsigned char *buf, size_t buf_size, bool bigend) :
	m_buf(buf),
	m_buf_size(buf_size),
	m_offset(0),
	m_bigend(bigend),
	m_hFile(NULL),
	m_hFileMapping(NULL)
{
}

DataStream::~DataStream()
{
	munmap_file();
}

void DataStream::munmap_file()
{
	if (m_hFileMapping != NULL)
	{
		int err = UnmapViewOfFile(m_buf);
		if (err == 0)
		{
			__DEBUG_LOG("munmap buf failed, error:%d", GetLastError());
		}
		if (m_hFileMapping != NULL)
		{
			CloseHandle(m_hFileMapping);
		}
		if (m_hFile != NULL)
		{
			CloseHandle(m_hFile);
		}
		if (m_buf != NULL)
		{
			m_buf = NULL;
		}
	}
}

int DataStream::mmap_file(const char *path)
{
	struct stat st = { 0 };
	if (stat(path, &st) != 0)
	{
		return -1;
	}

	munmap_file();
	m_hFile = CreateFile(LPCSTR(path),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		__DEBUG_LOG("ERROR: %u :unable to create file \"%s\".",
			GetLastError(),
			path);
		return -1;
	}

	m_hFileMapping = CreateFileMapping(
		m_hFile,
		NULL,
		PAGE_READONLY,
		0,
		0,        /*If this parameter and dwMaximumSizeHigh are 0 (zero),
					the maximum size of the file mapping object is equal
					to the current size of the file that hFile identifies.*/
		NULL);

	if (NULL == m_hFileMapping)
	{
		__DEBUG_LOG("could not create file mapping object :(%d)", GetLastError());
		return -1;
	}

	m_buf = (unsigned char*)MapViewOfFile(m_hFileMapping,
		FILE_MAP_READ,
		0,
		0,
		0);/*If this parameter is 0 (zero), the mapping extends from the specified offset to the end of the file mapping.*/
	if (m_buf == NULL)
	{
		__DEBUG_LOG("could not MapViewOfFile :(%d)", GetLastError());
		return -1;
	}

	m_buf_size = st.st_size;
	m_offset = 0;
	return 0;
}

void DataStream::reset_buf(unsigned char *buf, size_t buf_size)
{
	munmap_file();

	m_buf = buf;
	m_offset = 0;
	m_buf_size = buf_size;
}

void DataStream::set_offset(unsigned int offset)
{
	m_offset = offset;
}

uint8_t DataStream::read_u8()
{
	if (m_offset >= m_buf_size) return 0;
	return m_buf[m_offset++];
}

uint16_t DataStream::read_u16()
{
	uint16_t n1 = read_u8();
	uint16_t n2 = read_u8();

	if (m_bigend) return (n1 << 8) | n2;

	return n1 | (n2 << 8);
}

uint32_t DataStream::read_u32()
{
	uint32_t n1 = read_u16();
	uint32_t n2 = read_u16();

	if (m_bigend) return (n1 << 16) | n2;

	return n1 | (n2 << 16);
}

char *DataStream::read_str()
{
	uint32_t str_len = read_u32();
	if (m_offset + str_len <= m_buf_size)
	{
		char *buf = (char *)malloc(str_len + 1);
		if (str_len > 0)
		{
			memcpy(buf, m_buf + m_offset, str_len);
		}
		buf[str_len] = '\0';
		m_offset += str_len;
		return buf;
	}
	return NULL;
}

char *DataStream::read_enc_str()
{
	uint32_t str_len = read_u32();
	if (m_offset + str_len <= m_buf_size)
	{
		char *buf = (char *)malloc(str_len + 1);
		memcpy(buf, m_buf + m_offset, str_len);
		buf[str_len] = '\0';
		m_offset += str_len;

		//解密
		gt_decrypt_string((unsigned char *)buf, str_len);

		return buf;
	}
	return NULL;
}

unsigned char *DataStream::read_binary()
{
	uint32_t len = read_u32();
	if (len == 0 || m_offset + len > m_buf_size)
	{
		__DEBUG_LOG("len:%d m_offset:%d m_buf_size:%d", len, m_offset, m_buf_size);
		return NULL;
	}
	unsigned char *buf = (unsigned char *)malloc(len);
	if (buf == NULL)
	{
		__ASSERT(0);
		return NULL;
	}
	memcpy(buf, m_buf + m_offset, len);
	m_offset += len;

	return buf;
}

bool DataStream::write_u8(uint8_t u8)
{
	if (m_offset >= m_buf_size) return false;
	m_buf[m_offset] = u8;
	m_offset++;
	return true;
}

bool DataStream::write_u16(uint16_t u16)
{
	uint8_t num_1 = u16 & 0xff;
	uint8_t num_2 = (u16 >> 8) & 0xff;

	if (m_bigend)
	{
		if (!write_u8(num_2)) return false;
		if (!write_u8(num_1)) return false;
	}
	else
	{
		if (!write_u8(num_1)) return false;
		if (!write_u8(num_2)) return false;
	}

	return true;
}

bool DataStream::write_u32(uint32_t u32)
{
	uint16_t num_1 = u32 & 0xffff;
	uint16_t num_2 = (u32 >> 16) & 0xffff;

	if (m_bigend)
	{
		if (!write_u16(num_2)) return false;
		if (!write_u16(num_1)) return false;
	}
	else
	{
		if (!write_u16(num_1)) return false;
		if (!write_u16(num_2)) return false;
	}

	return true;
}

bool DataStream::write_str(const char *str)
{
	return write_binary((const unsigned char *)str, strlen(str));
}

bool DataStream::write_enc_str(const char *str)
{
	size_t len = strlen(str);

	//加密
	char *enc_str = (char *)malloc(len + 1);
	if (enc_str == NULL)
	{
		__ASSERT(0);
		return false;
	}
	strcpy(enc_str, str);
	gt_decrypt_string((unsigned char *)enc_str, len);

	bool retval = write_binary((const unsigned char *)enc_str, len);

	free(enc_str);
	return retval;
}

bool DataStream::write_binary(const unsigned char *data, size_t len)
{
	if (len + sizeof(int) + m_offset > m_buf_size) return false;

	write_u32(len);
	if (len > 0)
	{
		memcpy(m_buf + m_offset, data, len);
	}
	m_offset += len;

	return true;
}

void DataStream::free_buf(void *ptr)
{
	free(ptr);
}

