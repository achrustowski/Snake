#include "singly_linked_list.h"
#include <stdlib.h>
#include <stdio.h>

Node* NEW_NODE(int x, int y, int dir)
{
    Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->x = x;
    node->y = y;
    node->dir = dir;

    return node;
}

Linked_List* NEW_LINKED_LIST()
{
    Linked_List* list = (struct Linked_List*)malloc(sizeof(struct Linked_List));

    if (list == NULL)
    {
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;

    return list;
}

Node* LIST_GET_HEAD(Linked_List* list)
{
    return list->head;
}

Node* LIST_GET_TAIL(Linked_List* list)
{
    return list->tail;
}

void INSERT_AT_HEAD(Linked_List* list, Node* node)
{
    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
        return;
    }

    node->next = list->head;
    list->head = node;
    return;
}

void INSERT_AT_TAIL(Linked_List* list, Node* node)
{
    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
        return;
    }

    list->tail->next = node;
    list->tail = node;
    return;
}

void REMOVE_AT_HEAD(Linked_List* list)
{
    if (list->head == NULL)
    {
        printf("List is empty\n");
    }
    Node* temp = list->head;
    list->head = list->head->next;
    free(temp);
    temp = NULL;
}

void REMOVE_AT_TAIL(Linked_List* list)
{
    if (list->head == NULL)
    {
        printf("List is empty\n");
    } else if (list->head->next == NULL)
    {
        free(list->head);
        list->head = NULL;
    } else
    {
        Node* current = list->head;
        Node* previous = NULL;
        while (current->next != NULL)
        {
            previous = current;
            current = current->next;
        }
        previous->next = NULL;
        free(current);
        current = NULL;
    }
}

void PRINT_LIST(Linked_List* list)
{
    for (Node* node = list->head; node; node = node->next)
    {
        printf("%d %d\n", node->x, node->y);
    }
}

bool LIST_CONTAINS(Linked_List* list, Node* temp)
{
    for (Node* node = list->head; node; node = node->next)
    {
        if (node->x == temp->x && node->y == temp->y)
        {
            return true;
        }
    }
    return false;
}

void CLEAR_LIST(Linked_List* list)
{
    Node* temp = list->head;

    while (temp != NULL)
    {
        temp = temp->next;
        free(list->head);
        list->head = temp;
    }
}

void DELETE_LIST(Linked_List* list)
{
    CLEAR_LIST(list);

    free(list);
}
