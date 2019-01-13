#ifndef __MD5_H__
#define __MD5_H__
#include "common.h"
#include <stdlib.h>

#define MD5_LEN 32
#define MAX_MD5_LEN MD5_LEN*2

typedef unsigned char UInt8;
typedef unsigned int UInt32;

typedef struct MD5Context {
	UInt32 buf[4];
	UInt32 bits[2];
	unsigned char in[64];
} MD5Context;

void MD5Init(MD5Context *context);
void MD5Update(MD5Context *context, UInt8 *buf, unsigned int len);
void MD5Final(MD5Context *ctx, UInt8 *digest);
void MD5Transform(UInt32 *buf, UInt32 *in);

void GetStrMd5(const char *str, /*[out]*/char *md5);
void GetBufferMd5(unsigned char *buf, unsigned int len, /*[out]*/char *md5);
void GetBufferMd5Signature(const unsigned char *buf, size_t len, /*[out]*/unsigned char *md5_signature);
int GetFileMd5(const char *path, /*[out]*/char *md5);
int GetFileMd5Signature(const char *path, /*[out]*/unsigned char *md5_signature, size_t buf_size);
void HexMd5Signature(const UInt8 *signature, /*[out]*/char *md5);
int GetFileMd5SignatureLazy(const char *path, /*[out]*/unsigned char *md5_signature, size_t buf_size, size_t nanosleep_block, unsigned long sleep_ms);
int CompareMd5(const char* pre_md5, const char* pro_md5);
int GetFileMd5Wrapper(const char* path, std::string& md5);


#endif /* __MD5_H__ */

