#include <stdlib.h>
#include <string.h>

// ����ڵ�
struct Node
{
	void *data;
	Node *next;
};

// ����
struct List
{
	Node *head;
	Node *last;
	size_t size;
};

// ��������
List * create_list(size_t size);
// �������
Node * list_append(List * list, void * data);
// ��������
void destory_list(List * list);
// ��������
void traverse_list(List * list, void(*func)(void *));
