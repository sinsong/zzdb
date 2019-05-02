#include "zztb.h"
#include "zzutil.h"
#include <string.h>

int zz_create_table(zzConn * conn, const char * name, zzdb_tbdesc * desc, size_t count)
{
	// 检查表名
	if (zz_find_table(conn, name, NULL) == 0)
	{
		sprintf(zzdb_impl_error_message, "%s: table [%s] existed", __FUNCTION__, name);
		return -1;
	}

	char tbdesc_name[ZZ_FILENAME_MAX];
	char tbdata_name[ZZ_FILENAME_MAX];
	
	// 生成文件名字
	while (1)
	{
		zz_tmpname(tbdesc_name, ZZ_FILENAME_MAX);
		FILE *test = fopen(tbdesc_name, "r");
		if (test)
		{
			fclose(test);
		}
		else
		{
			break;
		}
	}

	while (1)
	{
		zz_tmpname(tbdata_name, ZZ_FILENAME_MAX);
		FILE *test = fopen(tbdata_name, "r");
		if (test)
		{
			fclose(test);
		}
		else
		{
			break;
		}
	}

	// 向系统表写入表的记录
	FILE *sys_tables = fopen(conn->filename_zzdb_tables, "ab");
	if (sys_tables == NULL)
	{
		sprintf(zzdb_impl_error_message, "%s: zzdb_tables.zzdb NOT FOUND.", __FUNCTION__);
		return -1;
	}
	zzdb_sys_table tb;
	tb.sysflag = zzdb_sys_exist;
	strcpy(tb.tbname, name);
	strcpy(tb.tbdesc, tbdesc_name);
	strcpy(tb.tbdata, tbdata_name);
	if (fwrite(&tb, sizeof(zzdb_sys_table), 1, sys_tables) != 1)
	{
		sprintf(zzdb_impl_error_message, "%s: zzdb_tables.zzdb fwrite failed.\n", __FUNCTION__);
	}
	fclose(sys_tables);
	
	char *tbdesc_fullname = zz_gen_fullpath(conn->dbpath, tbdesc_name);
	if (tbdesc_fullname == NULL)
	{
		return -1;
	}
	FILE *tbdesc = fopen(tbdesc_fullname, "wb");
	free(tbdesc_fullname);

	char *tbdata_fullname = zz_gen_fullpath(conn->dbpath, tbdata_name);
	if (tbdata_fullname == NULL)
	{
		return -1;
	}
	FILE *tbdata = fopen(tbdata_fullname, "wb");
	fclose(tbdata);
	free(tbdata_fullname);

	uint32_t offset = 0;
	uint32_t num = count;
	fwrite(&offset, sizeof(uint32_t), 1, tbdesc);
	fwrite(&num, sizeof(uint32_t), 1, tbdesc);
	for (uint32_t i = 0; i < count; ++i)
	{
		zztb_sys_tbdesc sysdesc;
		sysdesc.offset = offset;
		sysdesc.type = desc[i].type;
		strcpy(sysdesc.name, desc[i].name);
		if (desc[i].type == ZZTYPE::ZZT_STRING)
		{
			sysdesc.size = desc[i].optlen;
		}
		else
		{
			sysdesc.size = zzsize(desc[i].type);
		}

		fwrite(&sysdesc, sizeof(zztb_sys_tbdesc), 1, tbdesc);

		offset += sysdesc.size;
	}
	fseek(tbdesc, 0, SEEK_SET);
	fwrite(&offset, sizeof(uint32_t), 1, tbdesc);

	fclose(tbdesc);

	return 0;
}

int zz_delete_table(zzConn *conn, const char *name)
{
	FILE *sys_tables = fopen(conn->filename_zzdb_tables, "rb+");
	zzdb_sys_table tb;
	while (1)
	{
		long pos = ftell(sys_tables); // 因为需要位置来覆盖，所以重写不了。。。
		int ret = fread(&tb, sizeof(zzdb_sys_table), 1, sys_tables);
		if (ret > 0)
		{
			if ((strcmp(tb.tbname, name) == 0) && ((tb.sysflag & zzdb_sys_exist) != 0))
			{
				char *tbdesc_fullname = zz_gen_fullpath(conn->dbpath, tb.tbdesc);
				char *tbdata_fullname = zz_gen_fullpath(conn->dbpath, tb.tbdata);

				if (!remove(tbdesc_fullname)) sprintf(zzdb_impl_error_message, "%s: failed to remove tbdesc", __FUNCTION__);
				if (!remove(tbdata_fullname)) sprintf(zzdb_impl_error_message, "%s: failed to remove tbdata", __FUNCTION__);

				free(tbdesc_fullname);
				free(tbdata_fullname);

				tb.sysflag &= ~(zzdb_sys_exist); // 清除存在位
				fseek(sys_tables, pos, SEEK_SET);
				fwrite(&tb, sizeof(zzdb_sys_table), 1, sys_tables);

				fclose(sys_tables);
				return 0;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (feof(sys_tables))
			{
				fclose(sys_tables);
				return 0;
			}
			if (ferror(sys_tables))
			{
				fclose(sys_tables);
				return -1;
			}
		}
	}
}

int zz_find_table(zzConn *conn, const char *name, zzdb_sys_table *ptb)
{
	int func_retval;
	FILE *sys_tables = fopen(conn->filename_zzdb_tables, "rb");
	zzdb_sys_table tb;
	while (1)
	{
		int ret = fread(&tb, sizeof(zzdb_sys_table), 1, sys_tables);
		if (ret > 0)
		{
			if ((strcmp(tb.tbname, name) == 0) && (tb.sysflag & zzdb_sys_exist))
			{
				if(ptb)
					memcpy(ptb, &tb, sizeof(zzdb_sys_table));
				func_retval = 0;
				break;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (feof(sys_tables))
			{
				func_retval = -1;
				break;
			}
			if (ferror(sys_tables))
			{
				func_retval = -1;
				break;
			}
		}
	}

	fclose(sys_tables);
	return func_retval;
}

uint32_t zzsize(ZZTYPE type)
{
	switch (type)
	{
	case ZZT_INT:
		return sizeof(zz_int);
	case ZZT_BIGINT:
		return sizeof(zz_bigint);
	case ZZT_BOOL:
		return sizeof(zz_bool);
	case ZZT_FLOAT:
		return sizeof(zz_float);
	case ZZT_DOUBLE:
		return sizeof(zz_double);
	default:
		return 0;
	}
}