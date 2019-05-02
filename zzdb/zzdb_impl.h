#pragma once

#include "zzdef.h"
#include <stddef.h>
#include <stdint.h>

extern char zzdb_impl_error_message[50];

// 命名限制
#define NAME_MAX 30
#define ZZ_FILENAME_MAX 10

// sysflag
enum sysflag : uint8_t
{
	zzdb_sys_exist = 1U << 3, // 00001000
};

// 系统文件------------------------------------------

// zzdb.meta
#define ZZDB_META 0x42U

// zzdb_tables.zzdb

// 系统的表描述
struct zzdb_sys_table
{
	uint8_t sysflag;
	char tbname[NAME_MAX]; // 表名
	char tbdesc[ZZ_FILENAME_MAX]; // 表描述文件名
	char tbdata[ZZ_FILENAME_MAX]; // 表数据文件名
};

// zzdb_stat.zzdb
//
// dcount - dirty count

// 系统的表统计
struct zzdb_sys_stat
{
	char tbname[NAME_MAX];
	uint32_t dcount;
};

// 表描述文件
// 文件名字请查系统表 zzdb_tables.zzdb

// uint32_t tblen; 整个数据项长度
// uint32_t num; 列个数
struct zztb_sys_tbdesc
{
	uint32_t offset;
	uint32_t size;
	ZZTYPE type;
	char name[NAME_MAX];
};