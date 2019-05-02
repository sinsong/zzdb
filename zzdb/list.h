#include <stdlib.h>
#include <string.h>

// 链表节点
struct Node
{
	void *data;
	Node *next;
};

// 链表
struct List
{
	Node *head;
	Node *last;
	size_t size;
};

// 创建链表
List * create_list(size_t size);
// 添加数据
Node * list_append(List * list, void * data);
// 销毁链表
void destory_list(List * list);
// 遍历链表
void traverse_list(List * list, void(*func)(void *));
