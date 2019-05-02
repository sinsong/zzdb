#include "list.h"

List *create_list(size_t size)
{
	List *list = (List *)malloc(sizeof(List));
	if (list == NULL)
	{
		return NULL;
	}

	list->head = NULL;
	list->last = NULL;
	list->size = size;

	return list;
}


Node *list_append(List *list, void *data)
{
	Node *newnode = (Node *)malloc(sizeof(Node));
	if (newnode == NULL)
	{
		return NULL;
	}

	void *d = malloc(list->size);
	if (d == NULL)
	{
		free(newnode);
		return NULL;
	}

	newnode->data = d;
	memcpy(newnode->data, data, list->size);
	newnode->next = NULL;

	// 空列表情况
	if (list->head == NULL)
	{
		list->head = newnode;
		list->last = newnode;
		return newnode;
	}

	list->last->next = newnode;
	list->last = newnode;

	return newnode;
}

void destory_list(List *list)
{
	Node *cur = list->head;
	Node *temp = list->head;
	while (cur != NULL)
	{
		temp = cur;
		free(cur->data);
		cur = cur->next;
		free(temp);
	}
	free(list);
}

void traverse_list(List *list, void(*func)(void *))
{
	Node *cur = list->head;
	while (cur != NULL)
	{
		func(cur->data);

		cur = cur->next;
	}
}
