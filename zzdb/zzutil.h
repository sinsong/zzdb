#pragma once

#include <stdlib.h>
#include <memory>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

// 分配内存和字符串连接
char *zz_gen_fullpath(const char *a, const char *b);
// 生成临时文件名
void zz_tmpname(char *str, size_t size);