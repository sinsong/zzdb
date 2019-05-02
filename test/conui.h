#pragma once

// 询问输入
// [name] >>>
void input_ask(const char *name);
// 获取int
int input_int(const char *name, int *var);

// 菜单描述项
struct menu_item
{
	int id;
	const char *desc;
};

// 显示菜单，返回选择
// -1 -> error
int menu_display(const char *name, menu_item* items, size_t num);