#ifndef __DATA_STREAM_H__
#define __DATA_STREAM_H__
#include "common.h"

#define MAPPINGSIZE 4096

class DataStream
{
public:
	DataStream(unsigned char *buf, size_t buf_size, bool bigend = true);
	virtual ~DataStream();

public:
	int mmap_file(const char *path);
	void reset_buf(unsigned char *buf, size_t buf_size);
	void set_offset(unsigned int offset);

	uint8_t read_u8();
	uint16_t read_u16();
	uint32_t read_u32();
	char *read_str();               //读取字符串，由调用者负责内存清理
	char *read_enc_str();           //读取加密字符串, 由调用者负责内存释放
	unsigned char *read_binary();  //读取二进制数据，由调用者负责内存释放

	bool write_u8(uint8_t u8);
	bool write_u16(uint16_t u16);
	bool write_u32(uint32_t u32);
	bool write_str(const char *str);
	bool write_enc_str(const char *str);
	bool write_binary(const unsigned char *data, size_t len);

	const unsigned char *get_buf() const { return m_buf; };
	size_t get_buf_size() const { return m_buf_size; }
	size_t get_used_length() const { return m_offset; }

	void free_buf(void *ptr);

private:
	void munmap_file();

private:
	unsigned char *m_buf;
	size_t m_offset;
	size_t m_buf_size;

	bool m_bigend;

	HANDLE  m_hFile;
	HANDLE  m_hFileMapping;
};

#endif

