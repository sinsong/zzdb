#pragma once

// ѯ������
// [name] >>>
void input_ask(const char *name);
// ��ȡint
int input_int(const char *name, int *var);

// �˵�������
struct menu_item
{
	int id;
	const char *desc;
};

// ��ʾ�˵�������ѡ��
// -1 -> error
int menu_display(const char *name, menu_item* items, size_t num);