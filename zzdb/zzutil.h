#pragma once

#include <stdlib.h>
#include <memory>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

// �����ڴ���ַ�������
char *zz_gen_fullpath(const char *a, const char *b);
// ������ʱ�ļ���
void zz_tmpname(char *str, size_t size);