#ifndef __CRC_32_H__734__
#define __CRC_32_H__734__
#include <stdint.h>
#include "common.h"

unsigned int ssh_crc32(const unsigned char *s, unsigned int len);
unsigned int ssh_crc32_ex(const unsigned char *s, unsigned int len, unsigned int init);
unsigned int crc32_file(const char* filename);
int ssh_crc32_file(const char* file_path, unsigned int* crc);

#endif