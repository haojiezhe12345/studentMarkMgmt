#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define db_undergraduate "db_undergraduate.bin"
#define db_postgraduate "db_postgraduate.bin"

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
} nodeValue;

typedef struct node
{
    nodeValue value;
    struct node *next;
} node;

node *getLastNode(node *head)
{
    node *p = head;
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
        if (i == index)
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

void appendNode(node **head, nodeValue value)
{
    node *p = (node *)malloc(sizeof(node));
    p->value = value;
    p->next = NULL;
    node *pLast = getLastNode(*head);
    if (pLast == NULL)
    {
        *head = p;
    }
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
    if (index == 0)
    {
        node *p = (node *)malloc(sizeof(node));
        p->value = value;
        p->next = *head;
        *head = p;
        return;
    }
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
    if (index == 0)
    {
        node *pDel = *head;
        *head = (*head)->next;
        free(pDel);
        return;
    }
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
    if (fp == NULL)
    {
        return -1;
    }

    destroyList(head);
    nodeValue v;
    while (fread(&v, sizeof(v), 1, fp) == 1)
    {
        appendNode(head, v);
    }
    fclose(fp);
    return 0;
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

void bubbleSort(node **head)
{
    node *p = *head;
    for (int i = 0; p->next != NULL; i++)
    {
        node *p1 = *head;
        for (int j = 0; p1->next != NULL; j++)
        {
            if (p1->value.id > p1->next->value.id)
            {
                swapNodeValues(head, j, j + 1);
            }
            p1 = p1->next;
        }
        p = p->next;
    }
}

// input a string from stdin, returns the string
char *inputStr(const char *msg, int len)
{
    printf(msg);
    // to let fgets() read 1 char from stdin, you must give it a length of 3, because '\n\0' is counted
    // here, 'len + 2' is added to prevent overflow errors
    char *str = (char *)malloc(sizeof(char) * (len + 2));
    fgets(str, len + 2, stdin); // fixed length, may cause buffer overflow! consider doubling malloc when EOF not found
    // remove the '\n' at the end
    if (str[strlen(str) - 1] == '\n')
    {
        str[strlen(str) - 1] = '\0';
    }
    return str;
}

// input a number between range
unsigned long long inputNum(const char *msg, unsigned long long min, unsigned long long max)
{
    unsigned long long num;
    while (1)
    {
        char *str = inputStr(msg, 32);
        sscanf(str, "%llu", &num);
        if ((min <= num && num <= max))
        {
            break;
        }
        else
        {
            printf("Value should be between %llu and %llu!\n", min, max);
        }
    }
    return num;
}

// input asking for a selection (max 9 choices)
int inputSelect(const char *msg, int range)
{
    char *ch;
    while (1)
    {
        ch = inputStr(msg, 1);
        if ('1' <= *ch && *ch <= '0' + range)
        {
            break;
        }
        else
        {
            printf("Invalid choice!\n");
        }
    }
    return *ch - '0';
}

// input confirmation (y/N)
int inputConfirm(const char *msg)
{
    char *ch = inputStr(msg, 1);
    if (*ch == 'y' || *ch == 'Y')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int main()
{
    node *stu_undergraduate = NULL;
    node *stu_postgraduate = NULL;

    loadNodes(db_undergraduate, &stu_undergraduate);
    loadNodes(db_postgraduate, &stu_postgraduate);

    printf("Type \"h\" or \"help\" for help\n");
    while (1)
    {
        char *cmd = inputStr("> ", 32);

        if (cmd[0] == '\0')
        {
            continue;
        }
        else if (strcmp(cmd, "h") == 0 || strcmp(cmd, "help") == 0)
        {
            printf("h, help          Show help\n");
            printf("l, ls, list      List all students\n");
            printf("a, add           Add student\n");
            printf("m, mod, modify   Modify student\n");
            printf("d, del, delete   Delete student\n");
            printf("i, info          Get student info\n");

            printf("max              Show the person with the max wage\n");
            printf("avg-up           Show person count whose wage is more than average\n");
            printf("stress           Add N persons for stress test\n");
            printf("q, quit, exit    Exit program\n");
        }

        // student list
        else if (strcmp(cmd, "l") == 0 || strcmp(cmd, "ls") == 0 || strcmp(cmd, "list") == 0)
        {
            node *p = stu_undergraduate;
            while (1)
            {
                printf("%llu\t%s\n", p->value.id, p->value.name);
                if (p->next == NULL)
                {
                    if (p == getLastNode(stu_undergraduate))
                    {
                        p = stu_postgraduate;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    p = p->next;
                }
            }
        }

        // student add
        else if (strcmp(cmd, "a") == 0 || strcmp(cmd, "add") == 0)
        {
            int stuType = inputSelect("1. Undergraduate\n2. Postgraduate\nSelect student type: ", 2);

            nodeValue stu;

            // basic info
            strcpy(stu.name, inputStr("Name: ", 32));
            stu.gender = inputSelect("Gender (1=Male, 2=Female): ", 2);
            strcpy(stu.major, inputStr("Major: ", 64));
            stu.classid = inputNum("Class: ", 1, 99);

            // postgraduate-specified info
            if (stuType == 2)
            {
                strcpy(stu.direction, inputStr("Research direction: ", 64));
                strcpy(stu.tutor, inputStr("Tutor: ", 32));
            }
            else
            {
                stu.direction[0] = '\0';
                stu.tutor[0] = '\0';
            }

            // get current time
            time_t now = time(NULL);
            struct tm *time = localtime(&now);

            // student id format: year[4] + type[1] + major[3] + class[2] + order[2]
            stu.id = (time->tm_year + 1900) * 100000000ULL + stuType * 10000000ULL + 0 * 10000 + stu.classid * 100 + 1;

            // init marks
            stu.mark_math = -1;
            stu.mark_eng = -1;
            stu.mark_c = -1;
            stu.mark_overall = -1;
            stu.mark_paper = -1;

            if (stuType == 1)
            {
                appendNode(&stu_undergraduate, stu);
                saveNodes(db_undergraduate, stu_undergraduate);
            }
            else
            {
                appendNode(&stu_postgraduate, stu);
                saveNodes(db_postgraduate, stu_postgraduate);
            }
            printf("Student added successfully!\n");
        }

        else if (strcmp(cmd, "d") == 0 || strcmp(cmd, "del") == 0 || strcmp(cmd, "delete") == 0)
        {
            unsigned long long id = inputNum("Enter student ID: ", 0, 999999999999ULL);

            node *p = stu_undergraduate;
            node **pCurrentList = &stu_undergraduate;
            int index = 0;
            while (1)
            {
                if (p->value.id == id)
                {
                    printf("Delete student %llu %s? (y/N): ", p->value.id, p->value.name);
                    if (inputConfirm(""))
                    {
                        deleteNodeAtIndex(pCurrentList, index);
                        printf("Student deleted successfully!\n");
                    }
                    else
                    {
                        printf("Student not deleted\n");
                    }
                    break;
                }
                if (p->next == NULL)
                {
                    if (*pCurrentList == stu_undergraduate)
                    {
                        p = stu_postgraduate;
                        pCurrentList = &stu_postgraduate;
                        index = 0;
                    }
                    else
                    {
                        printf("Student not found\n");
                        break;
                    }
                }
                else
                {
                    p = p->next;
                    index++;
                }
            }
        }

        else if (strcmp(cmd, "c") == 0 || strcmp(cmd, "clear") == 0)
        {
            destroyList(&stu_undergraduate);
            destroyList(&stu_postgraduate);
        }

        else if (strcmp(cmd, "s") == 0 || strcmp(cmd, "save") == 0)
        {
            saveNodes(db_undergraduate, stu_undergraduate);
            saveNodes(db_postgraduate, stu_postgraduate);
        }

        else if (strcmp(cmd, "l") == 0 || strcmp(cmd, "load") == 0)
        {
            loadNodes(db_undergraduate, &stu_undergraduate);
            loadNodes(db_postgraduate, &stu_postgraduate);
        }

        else if (strcmp(cmd, "batch") == 0)
        {
            int count = inputNum("Number of students to add: ", 0, 99999999);
            for (int i = 0; i < count; i++)
            {
                printf("Adding %d / %d\r", i + 1, count);
                nodeValue stu;
                sprintf(stu.name, "test%d", i);
                appendNode(&stu_undergraduate, stu);
            }
            printf("\n");
        }

        else if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0)
        {
            break;
        }

        else
        {
            printf("Command not recognized\n");
        }
    }
}