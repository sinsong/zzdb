#include "zzcur.h"

zzCursor zz_open_table(zzConn* conn, const char *name)
{
	// 初始化
	zzCursor cur;
	cur.tbdesc = NULL;
	cur.tbsize = 0;
	cur.tbnum = 0;
	cur.refconn = NULL;

	// 这里设置tb
	if (zz_find_table(conn, name, &cur.tb) != 0)
	{
		sprintf(zzdb_impl_error_message, "%s: cannot find table", __FUNCTION__);
		cur.tbdesc = NULL;
		cur.tbsize = 0;
		cur.tbnum = 0;
		cur.refconn = NULL;
		return cur;
	}

	char *tbdesc_fullname = zz_gen_fullpath(conn->dbpath, cur.tb.tbdesc);
	if (tbdesc_fullname == NULL)
	{
		cur.tbdesc = NULL;
		cur.tbsize = 0;
		cur.tbnum = 0;
		cur.refconn = NULL;
		return cur;
	}
	FILE *tbdesc = fopen(tbdesc_fullname, "rb");
	free(tbdesc_fullname);

	if (!tbdesc)
	{
		// 文件损坏
		sprintf(zzdb_impl_error_message, "%s: tbdesc: %s broken", __FUNCTION__, cur.tb.tbdesc);
		cur.tbdesc = NULL;
		cur.tbsize = 0;
		cur.tbnum = 0;
		cur.refconn = NULL;
		return cur;
	}

	if (fread(&cur.tbsize, sizeof(uint32_t), 1, tbdesc) != 1)
	{
		// 文件还是有问题
		sprintf(zzdb_impl_error_message, "%s: tbdesc: %s broken", __FUNCTION__, cur.tb.tbdesc);
		cur.tbdesc = NULL;
		cur.tbsize = 0;
		cur.tbnum = 0;
		cur.refconn = NULL;
		return cur;
	}

	if (fread(&cur.tbnum, sizeof(uint32_t), 1, tbdesc) != 1)
	{
		// 文件还是有问题
		sprintf(zzdb_impl_error_message, "%s: tbdesc: %s broken", __FUNCTION__, cur.tb.tbdesc);
		cur.tbdesc = NULL;
		cur.tbsize = 0;
		cur.tbnum = 0;
		cur.refconn = NULL;
		return cur;
	}

	cur.tbdesc = (zztb_sys_tbdesc *)malloc(sizeof(zztb_sys_tbdesc) * cur.tbnum);
	if (cur.tbdesc == NULL)
	{
		cur.tbdesc = NULL;
		cur.tbsize = 0;
		cur.tbnum = 0;
		cur.refconn = NULL;
		return cur;
	}

	int readcount = fread(cur.tbdesc, sizeof(zztb_sys_tbdesc), cur.tbnum, tbdesc);
	if (readcount != cur.tbnum)
	{
		sprintf(zzdb_impl_error_message, "%s: read num != cur.tbnum", __FUNCTION__);
		cur.tbdesc = NULL;
		cur.tbsize = 0;
		cur.tbnum = 0;
		return cur;
	}

	cur.refconn = conn;

	fclose(tbdesc);

	return cur;
}

void zz_close_cursor(zzCursor cur)
{
	memset(&cur.tb, 0, sizeof(zzdb_sys_table));
	free(cur.tbdesc);
	cur.tbdesc = NULL;
	cur.tbsize = 0;
	cur.tbnum = 0;
	// refconn 不管
}

// 实际存储格式

// uint8_t sysflag
// zzdb_sys_tbdesc 描述的东西

int zz_cur_insert(zzCursor cur, const char * const names[], const void * objs[], size_t num)
{
	char *tbdata_fullname = zz_gen_fullpath(cur.refconn->dbpath, cur.tb.tbdata);
	if (tbdata_fullname == NULL)
	{
		return -1;
	}
	FILE *tbdata = fopen(tbdata_fullname, "ab+");
	free(tbdata_fullname);
	if (tbdata == NULL)
	{
		sprintf(zzdb_impl_error_message, "%s: tbdata fopen failed - %s", __FUNCTION__, cur.tb.tbdata);
		return -1;
	}

	size_t entry_size = sizeof(uint8_t) + cur.tbsize; // 记录项长度
	uint8_t *entry =(uint8_t *) malloc(entry_size);   // 记录项
	memset(entry, 0, entry_size);

	*entry |= zzdb_sys_exist; // 存在标记
	
	uint8_t *entry_data = entry + 1; // 实际数据

	for (size_t i = 0; i < num; ++i)
	{
		int rowpos = -1;
		// 查找对应列
		for (uint32_t j = 0; j < cur.tbnum; ++j)
		{
			if (strcmp(names[i], cur.tbdesc[j].name) == 0)
			{
				rowpos = j;
				break;
			}
		}
		if (rowpos < 0) // 没找到对应列
		{
			sprintf(zzdb_impl_error_message, "%s: row [%s] not found", __FUNCTION__, names[i]);
			continue;
		}

		uint8_t *entry_cur = entry_data + cur.tbdesc[rowpos].offset; // 移动指针
		memcpy(entry_cur, objs[i], cur.tbdesc[rowpos].size);
	}

	if (fwrite(entry, entry_size, 1, tbdata) != 1)
	{
		sprintf(zzdb_impl_error_message, "%s: tbdata fwrite failed - %s", __FUNCTION__, cur.tb.tbdata);
	}

	free(entry);
	fclose(tbdata);
	return 0;
}

// “私有”函数
zzResult *init_zzResult(zztb_sys_tbdesc *desc, uint32_t tbnum)
{
	zzResult *res = (zzResult *)malloc(sizeof(zzResult));
	if (res == NULL)
	{
		return NULL;
	}

	res->data = NULL;
	res->tbdesc = NULL;
	res->tbnum = 0;

	res->tbdesc = (zztb_sys_tbdesc *)malloc(sizeof(zztb_sys_tbdesc) * tbnum);
	if (res->tbdesc == NULL)
	{
		free(res);
		return NULL;
	}

	memcpy(res->tbdesc, desc, sizeof(zztb_sys_tbdesc) * tbnum);

	res->tbnum = tbnum;
	return res;
}

void zzClear(zzResult * res)
{
	if (res->data)
	{
		destory_list(res->data);
	}
	if (res->tbdesc)
	{
		free(res->tbdesc);
	}
	free(res);
}

int zz_cur_query_all_helper(void *) { return 1; }
zzResult *zz_cur_query_all(zzCursor cur)
{
	return zz_cur_query(cur, zz_cur_query_all_helper);
}

zzResult *zz_cur_query(zzCursor cur, int(*handler)(void *))
{
	zzResult *res = init_zzResult(cur.tbdesc, cur.tbnum);
	if (res == NULL)
	{
		return res;
	}

	char *tbdata_fullname = zz_gen_fullpath(cur.refconn->dbpath, cur.tb.tbdata);
	if (tbdata_fullname == NULL)
	{
		zzClear(res);
		return NULL;
	}
	FILE *tbdata = fopen(tbdata_fullname, "rb");
	free(tbdata_fullname);
	if (tbdata == NULL)
	{
		zzClear(res);
		return NULL;
	}
	if (tbdata == NULL)
	{
		zzClear(res);
		return NULL;
	}

	size_t entry_size = sizeof(uint8_t) + cur.tbsize;
	size_t readcount = 4096 / entry_size;
	size_t realread = 0;

	List *list = create_list(cur.tbsize);
	if (list == NULL)
	{
		zzClear(res);
		fclose(tbdata);
		return NULL;
	}

	while (1)
	{
		uint8_t *buff = (uint8_t *)malloc(entry_size * readcount);
		if (buff == NULL)
		{
			zzClear(res);
			fclose(tbdata);
			return NULL;
		}
		memset(buff, 0, entry_size * readcount);
		realread = fread(buff, entry_size, readcount, tbdata);
		if (realread == 0)
		{
			//if (feof(tbdata))
			//{
			//	break; // 正常退出
			//}
			break; // 还是得退出。。。
		}

		// 拷贝数据
		for (size_t i = 0; i < realread; ++i)
		{
			uint8_t *entry = buff + (entry_size * i);
			if (!(*entry & zzdb_sys_exist))
			{
				continue; // 该元素没有存在标记 （被删除）
			}

			uint8_t *data = entry + 1;
			if (handler(data))
			{
				list_append(list, data);
			}

		}

		if (realread < readcount)
		{
			break; // 读完了
		}

		free(buff);
	}
	res->data = list;

	fclose(tbdata);

	return res;
}

int32_t get_row_offset(zzCursor cur, const char *name)
{
	int32_t offset = -1;
	// 查找对应列
	for (uint32_t i = 0; i < cur.tbnum; ++i)
	{
		if (strcmp(name, cur.tbdesc[i].name) == 0)
		{
			offset = cur.tbdesc[i].offset;
			break;
		}
	}
	return offset;
}

int zz_cur_delete(zzCursor cur, int(*handler)(void *))
{
	char *tbdata_fullname = zz_gen_fullpath(cur.refconn->dbpath, cur.tb.tbdata);
	if (tbdata_fullname == NULL)
	{
		return -1;
	}
	FILE *tbdata = fopen(tbdata_fullname, "rb+");
	free(tbdata_fullname);
	// 晚一点检查，字符串的内存就已经释放了
	if (tbdata == NULL)
	{
		return -1;
	}

	size_t entry_size = sizeof(uint8_t) + cur.tbsize;
	uint8_t *entry = (uint8_t *)malloc(entry_size);
	uint8_t *data = entry + 1;
	if (entry == NULL)
	{
		return -1;
	}

	int return_val = -1;

	while (1)
	{
		long pos = ftell(tbdata);
		int ret = fread(entry, entry_size, 1, tbdata);
		if (ret > 0)
		{
			if (*entry & zzdb_sys_exist)
			{
			}
			else
			{
				continue;
			}
			
			if (handler(data) != 0)
			{
				*entry &= ~(zzdb_sys_exist);
				if (fseek(tbdata, pos, SEEK_SET) != 0)
				{
					return_val = -1;
					break;
				}
				fwrite(entry, entry_size, 1, tbdata);
				return_val = 0;
				break;
			}
		}
		else
		{
			if (feof(tbdata))
			{
				return_val = 0;
				break;
			}
			if (ferror(tbdata))
			{
				return_val = -1;
				break;
			}
		}
	}

	free(entry);
	fclose(tbdata);
	return return_val;
}