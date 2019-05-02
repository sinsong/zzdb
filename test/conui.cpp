#include "conui.h"
#include <stdio.h>

void input_ask(const char *name)
{
	printf("[%s] >>> ", name);
}

int input_int(const char *name, int *var)
{
	// scanf -> ��ȡ�ַ�����
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

	// �˵���Ŀ
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

	// ѯ��ѡ��
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