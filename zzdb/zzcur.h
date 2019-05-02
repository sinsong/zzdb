#pragma once

#include "zzdb.h"
#include "zzdb_impl.h"
#include "zztb.h"
#include "zzutil.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 游标
typedef struct
{
	zzdb_sys_table tb;       // 表信息   | 普通对象，拷贝就行
	zztb_sys_tbdesc *tbdesc; // 表描述符 | 动态内存！！！
	uint32_t tbsize;         // 表尺寸   | 0 为无效
	uint32_t tbnum;          // 表项目个数

	zzConn *refconn;  // 指向连接指针
} zzCursor;

// 查询结果
typedef struct
{
	List *data; // 记得只要数据，别读系统列
	zztb_sys_tbdesc *tbdesc;
	uint32_t tbnum;
} zzResult;

// 释放 zzResult
void zzClear(zzResult * res);

// 打开表（新建游标）
zzCursor zz_open_table(zzConn *conn, const char *name);
// 关闭游标（表）
void zz_close_cursor(zzCursor cur);

// 增
int zz_cur_insert(zzCursor cur, const char *const names[], const void *objs[], size_t num);

// 删
// return 1 (val != 0) to delete
// return 0 (val == 0) to ignore
int zz_cur_delete(zzCursor cur, int(*handler)(void *));

// 查
zzResult *zz_cur_query_all(zzCursor cur);
// return 1 (val != 0) to enable
// return 0 (val == 0) to disable
zzResult *zz_cur_query(zzCursor cur, int(*handler)(void *));

// 工具
int32_t get_row_offset(zzCursor cur, const char *name);