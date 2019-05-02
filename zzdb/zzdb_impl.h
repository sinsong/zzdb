#pragma once

#include "zzdef.h"
#include <stddef.h>
#include <stdint.h>

extern char zzdb_impl_error_message[50];

// ��������
#define NAME_MAX 30
#define ZZ_FILENAME_MAX 10

// sysflag
enum sysflag : uint8_t
{
	zzdb_sys_exist = 1U << 3, // 00001000
};

// ϵͳ�ļ�------------------------------------------

// zzdb.meta
#define ZZDB_META 0x42U

// zzdb_tables.zzdb

// ϵͳ�ı�����
struct zzdb_sys_table
{
	uint8_t sysflag;
	char tbname[NAME_MAX]; // ����
	char tbdesc[ZZ_FILENAME_MAX]; // �������ļ���
	char tbdata[ZZ_FILENAME_MAX]; // �������ļ���
};

// zzdb_stat.zzdb
//
// dcount - dirty count

// ϵͳ�ı�ͳ��
struct zzdb_sys_stat
{
	char tbname[NAME_MAX];
	uint32_t dcount;
};

// �������ļ�
// �ļ��������ϵͳ�� zzdb_tables.zzdb

// uint32_t tblen; �����������
// uint32_t num; �и���
struct zztb_sys_tbdesc
{
	uint32_t offset;
	uint32_t size;
	ZZTYPE type;
	char name[NAME_MAX];
};