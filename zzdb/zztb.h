#pragma once

#include "zzdb.h"
#include "zzdb_impl.h"

// 表描述
typedef struct
{
	const char *name;
	ZZTYPE type;
	uint32_t optlen;
} zzdb_tbdesc;

// 创建表
int zz_create_table(zzConn *conn, const char *name, zzdb_tbdesc *desc, size_t count);
// 删除表
int zz_delete_table(zzConn *conn, const char *name);
// 查找表
int zz_find_table(zzConn *conn, const char *name, zzdb_sys_table *ptb);
// 获取类型大小
uint32_t zzsize(ZZTYPE type);