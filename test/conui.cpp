#include "conui.h"
#include <stdio.h>

void input_ask(const char *name)
{
	printf("[%s] >>> ", name);
}

int input_int(const char *name, int *var)
{
	// scanf -> 读取字符个数
	input_ask(name);
	return scanf("%d", var);
}

int menu_display(const char *name, menu_item* items, size_t num)
{
	//==========================
	for (int i = 0; i < 30; ++i)
	{
		putchar('=');
	}
	putchar('\n');

	if (name)
	{
		printf("%s\n", name);
	}

	//---------------------------
	for (int i = 0; i < 30; ++i)
	{
		putchar('-');
	}
	putchar('\n');

	// 菜单项目
	for (size_t i = 0; i < num; ++i)
	{
		printf("  [%2d] %s\n", items[i].id, items[i].desc);
	}

	//----------------------------
	for (int i = 0; i < 30; ++i)
	{
		putchar('-');
	}
	putchar('\n');

	// 询问选择
	int choice;
	input_ask("choice");
	int ret = scanf("%d", &choice);
	if (ret)
	{
		return choice;
	}
	else
	{
		return -1;
	}
}