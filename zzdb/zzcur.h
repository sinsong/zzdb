#pragma once

#include "zzdb.h"
#include "zzdb_impl.h"
#include "zztb.h"
#include "zzutil.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// �α�
typedef struct
{
	zzdb_sys_table tb;       // ����Ϣ   | ��ͨ���󣬿�������
	zztb_sys_tbdesc *tbdesc; // �������� | ��̬�ڴ棡����
	uint32_t tbsize;         // ��ߴ�   | 0 Ϊ��Ч
	uint32_t tbnum;          // ����Ŀ����

	zzConn *refconn;  // ָ������ָ��
} zzCursor;

// ��ѯ���
typedef struct
{
	List *data; // �ǵ�ֻҪ���ݣ����ϵͳ��
	zztb_sys_tbdesc *tbdesc;
	uint32_t tbnum;
} zzResult;

// �ͷ� zzResult
void zzClear(zzResult * res);

// �򿪱��½��α꣩
zzCursor zz_open_table(zzConn *conn, const char *name);
// �ر��α꣨��
void zz_close_cursor(zzCursor cur);

// ��
int zz_cur_insert(zzCursor cur, const char *const names[], const void *objs[], size_t num);

// ɾ
// return 1 (val != 0) to delete
// return 0 (val == 0) to ignore
int zz_cur_delete(zzCursor cur, int(*handler)(void *));

// ��
zzResult *zz_cur_query_all(zzCursor cur);
// return 1 (val != 0) to enable
// return 0 (val == 0) to disable
zzResult *zz_cur_query(zzCursor cur, int(*handler)(void *));

// ����
int32_t get_row_offset(zzCursor cur, const char *name);