#include "zzdb.h"
#include "conui.h"

// 数据库连接
zzConn *conn = NULL;

// square 表的描述
// tbname = square
zzdb_tbdesc desc[] = {
	{"id", ZZT_INT, 0},
	{"num", ZZT_INT, 0},
	{"square", ZZT_INT, 0}
};

// 数据偏移
int32_t tba = 0;
int32_t tbb = 0;
int32_t tbc = 0;

// 前向声明
void test_traverse_table();
void test_query_table();
void test_create_table();
void test_insert_default_table();
void test_insert_input_value();
void test_delete_table();
void test_delete_entry();

// 要查询的num
int query_num = 42;

// 要删除的id
int delete_id = -1;

// 程序入口点
int main()
{
	// 数据库路径
	conn = zzConnect("H:\\hehe\\");

	if (conn == NULL)
	{
		printf("错误：数据库连接失败。\n%s\n", zzErrorMessage());
		printf("是否初始化一个新的数据库？");
		input_ask("y|n");
		char c;
		scanf("%c", &c);
		if (c == 'y')
		{
			conn = zzInitDB("H:\\hehe\\");
			printf("连接：%s\n", conn ? "成功" : "失败");
			if (conn == NULL)
			{
				exit(-1);
			}
		}
		else
		{
			exit(-1);
		}
	}

	menu_item menu[] = {
	{1, "遍历表[square]"},
	{2, "查询表[square]"},
	{3, "创建表[square]"},
	{4, "将默认值插入表[square]"},
	{5, "自行输入值并插入表[square]"},
	{6, "删除表[square]"},
	{7, "根据id删除某个数据项"},
	{8, "强制重新初始化数据库"},
	{9, "退出！"}
	};

	while (1)
	{
		switch (menu_display("ZZDB 数据库管理系统功能示例 [square]表", menu, 9))
		{
		case 1:
			test_traverse_table();
			break;
		case 2:
			test_query_table();
			break;
		case 3:
			test_create_table();
			break;
		case 4:
			test_insert_default_table();
			break;
		case 5:
			test_insert_input_value();
			break;
		case 6:
			test_delete_table();
			break;
		case 7:
			test_delete_entry();
			break;
		case 8:
			zzClose(conn);
			conn = zzInitDB("H:\\hehe\\");
			printf("初始化数据库：%s\n", conn ? "成功" : "失败");
			break;
		case 9:
			zzClose(conn);
			exit(0);
			break;
		default:
			printf("错误输入！重试！\n");
		}
		printf("按[Enter]键继续。。。");
		getchar();
		getchar();
	}

	zzClose(conn);
}

void test_print_table_data(void *data)
{
	uint8_t *cur = (uint8_t *)data;
	int *a = (int *)(cur + tba);
	int *b = (int *)(cur + tbb);
	int *c = (int *)(cur + tbc);

	printf("(%5d, %5d, %5d)\n", *a, *b, *c);
}

void test_traverse_table()
{
	zzCursor cur = zz_open_table(conn, "square");
	printf("zz_open_table: %s\n", cur.tbdesc ? "success" : "failed");
	if (cur.tbdesc == NULL)
		return;

	zzResult *res = zz_cur_query_all(cur);
	if (res == NULL)
	{
		printf("查询失败。\n");
		zz_close_cursor(cur);
		return;
	}

	tba = get_row_offset(cur, "id");
	tbb = get_row_offset(cur, "num");
	tbc = get_row_offset(cur, "square");
	if (tba < 0 || tbb < 0 || tbc < 0)
	{
		printf("get_row_offset failed.\n");
		zzClear(res);
		zz_close_cursor(cur);
		return;
	}

	traverse_list(res->data, test_print_table_data);

	zzClear(res);

	zz_close_cursor(cur);
}

int test_query_table_helper(void *data)
{
	uint8_t *cur = (uint8_t *)data;
	int *b = (int *)(cur + tbb);
	return (*b == query_num) ? 1 : 0;
}

void test_query_table()
{
	zzCursor cur = zz_open_table(conn, "square");
	printf("zz_open_table: %s\n", cur.tbdesc ? "success" : "failed");
	if (cur.tbdesc == NULL)
		return;

	tbb = get_row_offset(cur, "num");
	if (tbb < 0)
	{
		printf("get_row_offset failed.\n");
		zz_close_cursor(cur);
		return;
	}

	while (!input_int("num", &query_num)) printf("bad value, try again.\n");

	zzResult *res = zz_cur_query(cur, test_query_table_helper);
	if (res == NULL)
	{
		printf("查询失败。\n");
		zz_close_cursor(cur);
		return;
	}


	traverse_list(res->data, test_print_table_data);

	zzClear(res);

	zz_close_cursor(cur);
}

void test_create_table()
{
	if (zz_create_table(conn, "square", desc, 3) == 0)
	{
		printf("创建表成功\n");
	}
	else
	{
		printf("创建表失败\n");
	}

	zzCursor cur = zz_open_table(conn, "square");
	printf("zz_open_table: %s\n\n", cur.tbdesc ? "success" : "failed");
	if (cur.tbdesc == NULL)
		return;

	printf("table: [%s]\n", cur.tb.tbname);
	printf("tblen: %u\n", cur.tbsize);
	printf("tbnum: %u\n", cur.tbnum);
	for (uint32_t i = 0; i < cur.tbnum; ++i)
	{
		printf("P+0x%3X [0x%3X] - %s\n", cur.tbdesc[i].offset, cur.tbdesc[i].size, cur.tbdesc[i].name);
	}
}

void test_insert_default_table()
{
	zzCursor cur = zz_open_table(conn, "square");
	printf("zz_open_table: %s\n", cur.tbdesc ? "success" : "failed");
	if (cur.tbdesc == NULL)
		return;

	const char *names[] = { "id", "num", "square" };

	//    id: 0   1   2...
	//   num: 1   2   3...
	//square: 1   4   6...
	for (int i = 0; i < 100; ++i)
	{
		int a = i;
		int b = i + 1;
		int c = b * b;
		const void *objs[] = { &a, &b, &c };
		if (zz_cur_insert(cur, names, objs, 3) != 0)
		{
			putchar('*');
		}
	}

	zz_close_cursor(cur);
}

void test_insert_input_value()
{
	zzCursor cur = zz_open_table(conn, "square");
	printf("zz_open_table: %s\n", cur.tbdesc ? "success" : "failed");
	if (cur.tbdesc == NULL)
		return;

	const char *names[] = { "id", "num", "square" };

	int a;
	int b;
	int c;
	const void *objs[] = { &a, &b, &c };

retry:
	while (!input_int("id", &a)) printf("错误的值，重试\n");
	while (!input_int("num", &b)) printf("错误的值，重试\n");
	while (!input_int("square", &c)) printf("错误的值，重试\n");

	printf("Insert tuple:\n(%d, %d, %d)\n", a, b, c);

	menu_item menu[] = {
	{1, "继续 [Default]"},
	{2, "重试"},
	};

	switch (menu_display("是否继续？", menu, 2))
	{
	case 1:
	default_choice:
		if (zz_cur_insert(cur, names, objs, 3) != 0)
		{
			printf("插入失败\n");
		}
		break;
	case 2:
		goto retry;
		break;
	default:
		goto default_choice;
		break;
	}

	zz_close_cursor(cur);
}

void test_delete_table()
{
	if (zz_delete_table(conn, "square") != 0)
	{
		printf("删除失败。\n");
	}
}

int test_delete_entry_helper(void *data)
{
	uint8_t *cur = (uint8_t *)data;
	int *data_a = (int *)(cur + tba);
	return (*data_a == delete_id) ? 1 : 0;
}

void test_delete_entry()
{
	zzCursor cur = zz_open_table(conn, "square");
	printf("zz_open_table: %s\n", cur.tbdesc ? "success" : "failed");
	if (cur.tbdesc == NULL)
		return;

	tba = get_row_offset(cur, "id");
	if (tba < 0)
	{
		printf("get_row_offset failed.\n");
		zz_close_cursor(cur);
		return;
	}

retry:
	while (!input_int("id", &delete_id)) printf("错误的值，重试\n");

	printf("Delete id: %d\n", delete_id);

	menu_item menu[] = {
	{1, "继续 [默认]"},
	{2, "重试"},
	};

	switch (menu_display("是否继续？", menu, 2))
	{
	case 1:
	default_choice:
		if (zz_cur_delete(cur, test_delete_entry_helper) != 0)
		{
			printf("删除失败\n");
		}
		break;
	case 2:
		goto retry;
		break;
	default:
		goto default_choice;
		break;
	}

	zz_close_cursor(cur);
}