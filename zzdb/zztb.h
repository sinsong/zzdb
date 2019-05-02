#pragma once

#include "zzdb.h"
#include "zzdb_impl.h"

// ������
typedef struct
{
	const char *name;
	ZZTYPE type;
	uint32_t optlen;
} zzdb_tbdesc;

// ������
int zz_create_table(zzConn *conn, const char *name, zzdb_tbdesc *desc, size_t count);
// ɾ����
int zz_delete_table(zzConn *conn, const char *name);
// ���ұ�
int zz_find_table(zzConn *conn, const char *name, zzdb_sys_table *ptb);
// ��ȡ���ʹ�С
uint32_t zzsize(ZZTYPE type);