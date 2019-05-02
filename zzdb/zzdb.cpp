#include "zzdb.h"
#include "zzdb_impl.h"
#include "zzutil.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

zzConn *zzInitDB(const char *path)
{
	zzConn *ret = NULL;

	// 先构造空的东西
	ret = (zzConn *)malloc(sizeof(zzConn));
	if (!ret)
		return NULL;
	ret->dbpath = NULL;
	ret->filename_zzdb_stat = NULL;
	ret->filename_zzdb_tables = NULL;

	// 尝试初始化
	ret->dbpath = (char *)malloc(strlen(path) + 1);
	if (!ret->dbpath)
	{
		sprintf(zzdb_impl_error_message, "%s: malloc failed.", __FUNCTION__);
		zzClose(ret);
		return NULL;
	}
	strcpy(ret->dbpath, path);

	// 数据库标志文件
	char *filename_meta = zz_gen_fullpath(ret->dbpath, "zzdb.meta");
	FILE *meta = fopen(filename_meta, "wb");
	if (!filename_meta || !meta)
	{
		sprintf(zzdb_impl_error_message, "%s: failed to write zzdb.meta .", __FUNCTION__);
		if (filename_meta) free(filename_meta);
		if (meta) fclose(meta);
		zzClose(ret);
		return NULL;
	}

	uint32_t magic = ZZDB_META;
	if (!fwrite(&magic, sizeof(uint32_t), 1, meta)) // 写魔法数字
	{
		sprintf(zzdb_impl_error_message, "%s: failed to write magic number to zzdb.meta .", __FUNCTION__);
		if (filename_meta) free(filename_meta);
		if (meta) fclose(meta);
		return zzClose(ret);
	}
	// 不用了释放掉
	if (filename_meta) free(filename_meta);
	if (meta) fclose(meta);

	// 构造文件名字
	ret->filename_zzdb_tables = zz_gen_fullpath(ret->dbpath, "zzdb_tables.zzdb");
	if (!ret->filename_zzdb_tables) return zzClose(ret);
	ret->filename_zzdb_stat = zz_gen_fullpath(ret->dbpath, "zzdb_stat.zzdb");
	if (!ret->filename_zzdb_stat) return zzClose(ret);

	// 创建文件
	FILE *file_zzdb_tables = fopen(ret->filename_zzdb_tables, "wb");
	if (!file_zzdb_tables) return zzClose(ret);
	FILE *file_zzdb_stat = fopen(ret->filename_zzdb_stat, "wb");
	if (!file_zzdb_stat) return zzClose(ret);

	// 关闭文件
	fclose(file_zzdb_tables);
	fclose(file_zzdb_stat);
	return ret;
}

zzConn *zzConnect(const char *path)
{
	zzConn *ret = NULL;

	// 先构造空的东西
	ret = (zzConn *)malloc(sizeof(zzConn));
	if (!ret)
		return NULL;
	ret->dbpath = NULL;
	ret->filename_zzdb_stat = NULL;
	ret->filename_zzdb_tables = NULL;

	// 尝试初始化
	ret->dbpath = (char *)malloc(strlen(path) + 1);
	if (!ret->dbpath)
	{
		sprintf(zzdb_impl_error_message, "%s: malloc failed.", __FUNCTION__);
		return zzClose(ret);
	}

	strcpy(ret->dbpath, path);

	// 读取数据库标志文件
	char *filename_meta = zz_gen_fullpath(ret->dbpath, "zzdb.meta");
	if (filename_meta == NULL)
	{
		return zzClose(ret);
	}
	FILE *meta = fopen(filename_meta, "rb");
	free(filename_meta);
	if (meta == NULL)
	{
		sprintf(zzdb_impl_error_message, "%s: zzdb.meta not found.", __FUNCTION__);
		return zzClose(ret);
	}
	uint32_t buffer;
	if (fread(&buffer, sizeof(uint32_t), 1, meta))
	{
		if (buffer != ZZDB_META) // 检查魔法数字
		{
			sprintf(zzdb_impl_error_message, "%s: magic number wrong", __FUNCTION__);
			return zzClose(ret);
		}
	}
	else
	{ // 读取失败
		sprintf(zzdb_impl_error_message, "%s: failed to read magic number in zzdb.meta .", __FUNCTION__);
		return zzClose(ret);
	}

	// 构造文件名字
	ret->filename_zzdb_tables = zz_gen_fullpath(ret->dbpath, "zzdb_tables.zzdb");
	if (!ret->filename_zzdb_tables) return zzClose(ret);
	ret->filename_zzdb_stat = zz_gen_fullpath(ret->dbpath, "zzdb_stat.zzdb");
	if (!ret->filename_zzdb_stat) return zzClose(ret);

	return ret;
}

zzConn *zzClose(zzConn *conn)
{
	if (conn->dbpath) free(conn->dbpath);
	if (conn->filename_zzdb_stat) free(conn->filename_zzdb_stat);
	if (conn->filename_zzdb_tables) free(conn->filename_zzdb_tables);

	free(conn);
	return NULL;
}

const char *zzErrorMessage()
{
	return zzdb_impl_error_message;
}