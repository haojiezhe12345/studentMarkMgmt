#ifndef linkedList
#define linkedList

#include <stdio.h>
#include <stdlib.h>

typedef struct nodeValue
{
    // base info
    unsigned long long id;
    char name[32];
    int gender;
    char major[64];
    int classid;
    // extra info for postgraduate
    char direction[64];
    char tutor[32];
    // undergraduate marks
    int mark_math;
    int mark_eng;
    int mark_c;
    // postgraduate marks
    int mark_overall;
    int mark_paper;
    // ranks
    int totalmarks;
    int rank_school;
    int rank_class;
} nodeValue;

typedef struct node
{
    nodeValue value;
    struct node *next;
} node;

node *getLastNode(node *head)
{
    node *p = head;
    // `p != NULL` checks if the head pointer is null (length = 0)
    while (p != NULL && p->next != NULL)
    {
        p = p->next;
    }
    return p;
}

node *getNodeAtIndex(node *head, int index)
{
    node *p = head;
    for (int i = 0; p != NULL; i++)
    {
        // found node
        if (i == index)
        {
            return p;
        }
        p = p->next;
    }
    // node not found
    return NULL;
}

int getNodesCount(node *head)
{
    node *p = head;
    int i;
    for (i = 0; p != NULL; i++)
    {
        p = p->next;
    }
    return i;
}

node **getPagedNodes(node *head, int size, int page)
{
    node *p = head;
    node **nodelist = (node **)calloc(size, sizeof(node *));
    int listIndex = 0;
    for (int i = 0; p != NULL && listIndex < size; i++)
    {
        if (i >= page * size)
        {
            nodelist[listIndex] = p;
            listIndex++;
        }
        p = p->next;
    }
    return nodelist;
}

void appendNode(node **head, nodeValue value)
{
    // init node
    node *p = (node *)malloc(sizeof(node));
    p->value = value;
    p->next = NULL;
    // get last node
    node *pLast = getLastNode(*head);
    // length = 0, add to head pointer
    if (pLast == NULL)
    {
        *head = p;
    }
    // length > 0, append to last node->next
    else
    {
        pLast->next = p;
    }
}

void insertNodeAfter(node *p0, nodeValue value)
{
    node *p = (node *)malloc(sizeof(node));
    p->value = value;
    p->next = p0->next;
    p0->next = p;
}

void insertNodeAtIndex(node **head, int index, nodeValue value)
{
    node *p = *head;
    // insert at head
    if (index == 0)
    {
        node *p = (node *)malloc(sizeof(node));
        p->value = value;
        p->next = *head;
        *head = p;
        return;
    }
    // insert at index (loop index `i` starts from 1)
    for (int i = 1; p != NULL; i++)
    {
        if (i == index)
        {
            insertNodeAfter(p, value);
            return;
        }
        p = p->next;
    }
}

void deleteNodeAfter(node *p)
{
    node *pDel = p->next;
    p->next = p->next->next;
    free(pDel);
}

void deleteNodeAtIndex(node **head, int index)
{
    node *p = *head;
    // if index = 0, delete first node
    if (index == 0)
    {
        node *pDel = *head;
        *head = (*head)->next;
        free(pDel);
        return;
    }
    // if index > 1, start looping from first node
    // `i` starts from 1, because we have to use deleteNodeAfter() to delete a node
    for (int i = 1; p->next != NULL; i++)
    {
        if (i == index)
        {
            deleteNodeAfter(p);
            return;
        }
        p = p->next;
    }
}

void destroyList(node **head)
{
    node *p = *head;
    *head = NULL;
    while (p != NULL)
    {
        node *pNext = p->next;
        free(p);
        p = pNext;
    }
}

void saveNodes(const char *file, node *head)
{
    FILE *fp = fopen(file, "wb");
    node *p = head;
    // dump all nodes from memory
    while (p != NULL)
    {
        fwrite(&p->value, sizeof(p->value), 1, fp);
        p = p->next;
    }
    fclose(fp);
}

int loadNodes(const char *file, node **head)
{
    FILE *fp = fopen(file, "rb");
    // file not found
    if (fp == NULL)
    {
        return 0;
    }

    // delete all old nodes
    destroyList(head);
    // load file into memory
    nodeValue v;
    while (fread(&v, sizeof(v), 1, fp) == 1)
    {
        appendNode(head, v);
    }
    fclose(fp);
    return 1;
}

void swapNodeValues(node **head, int index1, int index2)
{
    if (index1 > index2)
    {
        int temp = index1;
        index1 = index2;
        index2 = temp;
    }
    else if (index1 == index2)
    {
        return;
    }
    node *p = *head;
    node *p1 = NULL;
    nodeValue p2v;
    for (int i = 0; p != NULL; i++)
    {
        if (i == index1)
        {
            p1 = p;
        }
        if (i == index2)
        {
            p2v = p->value;
            p->value = p1->value;
            p1->value = p2v;
            return;
        }
        p = p->next;
    }
}

// sort a linked list by an `int` member value
// `order`: `1`=Ascending, `2`=Descending
void bubbleSortByIntValue(node **head, int *memberValue, int order)
{
    node *p = *head;
    size_t memberOffset = (size_t)memberValue - (size_t) & (**head).value;
    for (int i = 0; p->next != NULL; i++)
    {
        node *p1 = *head;
        for (int j = 0; p1->next != NULL; j++)
        {
            int v1 = *(int *)((size_t)&p1->value + memberOffset);
            int v2 = *(int *)((size_t)&p1->next->value + memberOffset);
            if ((order == 1 && v1 > v2) || (order == 2 && v1 < v2))
            {
                swapNodeValues(head, j, j + 1);
            }
            p1 = p1->next;
        }
        p = p->next;
    }
}

#endif
