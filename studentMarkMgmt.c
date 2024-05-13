#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

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
    // to let fgets() read 1 char from stdin, you must give it a length of 3, because `\n\0` is counted
    // here, `len + 2` is added to prevent overflow errors
    char *str = (char *)malloc(sizeof(char) * (len + 2));
    fgets(str, len + 2, stdin); // fixed length, may cause buffer overflow! consider doubling malloc when EOF not found
    // remove the `\n` at the end
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
        // str to int
        sscanf(str, "%llu", &num);
        // check range
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
        // check range
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

//
// functions specific to students
//

// declare node lists
node *stu_undergraduate = NULL;
node *stu_postgraduate = NULL;

// loop over students and calls `callback` function on each loop
// returns 1 if looped completely, 0 if interrupted
int students_forEach(int (*callback)(node **, node *, int, va_list), ...)
{
    // pointer for looping
    node *p = stu_undergraduate;
    // current working list
    node **pCurrentList = &stu_undergraduate;
    int index = 0;
    while (1)
    {
        if (p != NULL)
        {
            // start parsing varaible args
            va_list args;
            va_start(args, callback);
            // if callback returns 1, quit looping and return 0
            if (callback(pCurrentList, p, index, args))
            {
                return 0;
            }
            // clean up variable args
            va_end(args);
            // head to next node
            p = p->next;
            index++;
        }
        // reach end of list
        else
        {
            // switch to postgraduate list
            if (*pCurrentList == stu_undergraduate && stu_postgraduate != NULL)
            {
                p = stu_postgraduate;
                pCurrentList = &stu_postgraduate;
                index = 0;
            }
            else
            {
                break;
            }
        }
    }
    // loop fully completed
    return 1;
}

int student_print(node **pCurrentList, node *p, int index, va_list args)
{
    printf("%llu\t%s\t%s\t%s\t%d\n", p->value.id, p->value.name, p->value.gender == 1 ? "Male" : "Female", p->value.major, p->value.classid);
    return 0;
}

int student_delete(node **pCurrentList, node *p, int index, va_list args)
{
    if (p->value.id == va_arg(args, unsigned long long))
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
        return 1;
    }
    return 0;
}

int student_modify_info(node **pCurrentList, node *p, int index, va_list args)
{
    if (p->value.id == va_arg(args, unsigned long long))
    {
        char *str;

        printf("Name (leave blank for %s): ", p->value.name);
        str = inputStr("", 32);
        if (str[0] != '\0')
        {
            strcpy(p->value.name, str);
        }

        while (1)
        {
            printf("Gender (1=Male, 2=Female, leave blank for %s): ", p->value.gender == 1 ? "Male" : "Female");
            str = inputStr("", 1);
            if (str[0] == '1' || str[0] == '2')
            {
                p->value.gender = str[0] - '0';
                break;
            }
            // knwon issue: entering even length of string causes the last input to be `\0`, which leaves this unchanged
            else if (str[0] == '\0')
            {
                break;
            }
            else
            {
                printf("Invalid choice!\n");
            }
        }

        printf("Major (leave blank for %s): ", p->value.major);
        str = inputStr("", 64);
        if (str[0] != '\0')
        {
            strcpy(p->value.major, str);
        }

        while (1)
        {
            printf("Class (leave blank for %d): ", p->value.classid);
            str = inputStr("", 32);
            if (str[0] != '\0')
            {
                unsigned long long newid;
                sscanf(str, "%llu", &newid);
                if ((1 <= newid && newid <= 99))
                {
                    p->value.classid = newid;
                    break;
                }
                else
                {
                    printf("Value should be between 1 and 99!\n");
                }
            }
            else
            {
                break;
            }
        }

        if (*pCurrentList == stu_postgraduate)
        {
            printf("Research direction (leave blank for %s): ", p->value.direction);
            str = inputStr("", 64);
            if (str[0] != '\0')
            {
                strcpy(p->value.direction, str);
            }

            printf("Tutor (leave blank for %s): ", p->value.tutor);
            str = inputStr("", 32);
            if (str[0] != '\0')
            {
                strcpy(p->value.tutor, str);
            }
        }
        return 1;
    }
    return 0;
}

int student_modify_marks(node **pCurrentList, node *p, int index, va_list args)
{
    if (p->value.id == va_arg(args, unsigned long long))
    {

        return 1;
    }
    return 0;
}

int main()
{
    // load database from file
    printf("\nLoading %s ... ", db_undergraduate);
    printf("%s\n", loadNodes(db_undergraduate, &stu_undergraduate) ? "success" : "fail");
    printf("Loading %s ... ", db_postgraduate);
    printf("%s\n", loadNodes(db_postgraduate, &stu_postgraduate) ? "success" : "fail");

    printf("\nType \"h\" or \"help\" for help\n\n");
    // cmd input loop
    while (1)
    {
        char *cmds = inputStr("> ", 64);

        if (cmds[0] == '\0')
        {
            continue;
        }

        char cmd[64];
        sscanf(cmds, "%s", cmd);

        if (strcmp(cmd, "h") == 0 || strcmp(cmd, "help") == 0)
        {
            printf("h, help           Show help\n");
            printf("l, ls, list       List students\n");
            printf("a, add            Add student\n");
            printf("m, mod, modify    Modify student\n");
            printf("d, del, delete    Delete student\n");
            printf("save              Save databases to file\n");
            printf("load              Load databases from file\n");
            printf("q, quit, exit     Exit program\n");
        }

        // student list
        else if (strcmp(cmd, "l") == 0 || strcmp(cmd, "ls") == 0 || strcmp(cmd, "list") == 0)
        {
            students_forEach(student_print);
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

        // student modify
        else if (strcmp(cmd, "m") == 0 || strcmp(cmd, "mod") == 0 || strcmp(cmd, "modify") == 0)
        {
            char subcmd[64];
            unsigned long long id;
            int params = sscanf(cmds, "%s %s %llu", cmd, subcmd, &id);
            if (params == 3 && strcmp(subcmd, "info") == 0)
            {
                if (students_forEach(student_modify_info, id))
                {
                    printf("Student not found\n");
                }
            }
            else if (params == 3 && strcmp(subcmd, "mark") == 0)
            {
                if (students_forEach(student_delete, id))
                {
                    printf("Student not found\n");
                }
            }
            else
            {
                printf("modify info <Student ID>    Modify student's basic info\n");
                printf("modify mark <Student ID>    Modify student's marks\n");
            }
        }

        // student delete
        else if (strcmp(cmd, "d") == 0 || strcmp(cmd, "del") == 0 || strcmp(cmd, "delete") == 0)
        {
            unsigned long long id;
            if (sscanf(cmds, "%s %llu", cmd, &id) == 2)
            {
                if (students_forEach(student_delete, id))
                {
                    printf("Student not found\n");
                }
            }
            else
            {
                printf("Usage: delete <Student ID>\n");
            }
        }

        else if (strcmp(cmd, "clear") == 0)
        {
            destroyList(&stu_undergraduate);
            destroyList(&stu_postgraduate);
        }

        else if (strcmp(cmd, "save") == 0)
        {
            saveNodes(db_undergraduate, stu_undergraduate);
            saveNodes(db_postgraduate, stu_postgraduate);
        }

        else if (strcmp(cmd, "load") == 0)
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

        printf("\n");
    }
}