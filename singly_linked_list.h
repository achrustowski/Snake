#ifndef SINGLY_LINKED_LIST
#define SINGLY_LINKED_LIST

#include <stdbool.h>

typedef struct Node
{
    int x;
    int y;
    int dir;
    struct Node* next;
} Node;

typedef struct Linked_List
{
    Node* head;
    Node* tail;
} Linked_List;

//Create new node and return pointer to the new node
Node* NEW_NODE(int x, int y, int dir);

//Create new linked list and return pointer to the new linked list
Linked_List* NEW_LINKED_LIST();

//Get pointer to the head of the list
Node* LIST_GET_HEAD(Linked_List* linked_list);

//Get pointer to the tail of the list
Node* LIST_GET_TAIL(Linked_List* linked_list);

//Insert node at the head of the list
void INSERT_AT_HEAD(Linked_List* linked_list, Node* node);

//Insert node at the tail of the list
void INSERT_AT_TAIL(Linked_List* linked_list, Node* node);

//Remove head of the list, return pointer to the new head
void REMOVE_AT_HEAD(Linked_List* linked_list);

//Remove tail of the list, return pointer to the new tail
void REMOVE_AT_TAIL(Linked_List* linked_list);

//Print content of the linked list
void PRINT_LIST(Linked_List* linked_list);

bool LIST_CONTAINS(Linked_List* linked_list, Node* temp);

void CLEAR_LIST(Linked_List* linked_list);

void DELETE_LIST(Linked_List* linked_list);

#endif
