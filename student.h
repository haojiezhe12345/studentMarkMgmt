#ifndef students
#define students

#include <stdio.h>
#include <stdarg.h>

#include "linkedList.h"
#include "inputs.h"

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
    printf("%llu\t%s\t%s\t%s\tClass %d\n", p->value.id, p->value.name, p->value.gender == 1 ? "Male" : "Female", p->value.major, p->value.classid);
    return 0;
}

// delete student by id (no confirmation)
int student_delete(node **pCurrentList, node *p, int index, va_list args)
{
    if (p->value.id == va_arg(args, unsigned long long))
    {
        deleteNodeAtIndex(pCurrentList, index);
        return 1;
    }
    return 0;
}

// delete student by id (with confirmation)
int student_delete_confirm(node **pCurrentList, node *p, int index, va_list args)
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

int student_modify(node **pCurrentList, node *p, int index, va_list args)
{
    if (p->value.id == va_arg(args, unsigned long long))
    {
        reInputStr(p->value.name, "Name (leave blank for %s): ", 32);
        reInputSelect(&p->value.gender, "Gender (1=Male, 2=Female, leave blank for %d): ", 2);
        reInputStr(p->value.major, "Major (leave blank for %s): ", 64);
        reInputNum((unsigned long long *)&p->value.classid, "Class (leave blank for %d): ", 1, 99);

        if (*pCurrentList == stu_postgraduate)
        {
            reInputStr(p->value.direction, "Research direction (leave blank for %s): ", 64);
            reInputStr(p->value.tutor, "Tutor (leave blank for %s): ", 32);
        }
        return 1;
    }
    return 0;
}

int student_marks_show(node **pCurrentList, node *p, int index, va_list args)
{
    if (p->value.id == va_arg(args, unsigned long long))
    {
        student_print(pCurrentList, p, index, args);
        if (*pCurrentList == stu_undergraduate)
        {
            printf("Math:          %d\n", p->value.mark_math);
            printf("English:       %d\n", p->value.mark_eng);
            printf("C Programming: %d\n", p->value.mark_c);
        }
        else if (*pCurrentList == stu_postgraduate)
        {
            printf("Overall: %d\n", p->value.mark_overall);
            printf("Paper:   %d\n", p->value.mark_paper);
        }
        return 1;
    }
    return 0;
}

int student_marks_update(node **pCurrentList, node *p, int index, va_list args)
{
    if (p->value.id == va_arg(args, unsigned long long))
    {
        if (*pCurrentList == stu_undergraduate)
        {
            reInputNum((unsigned long long *)&p->value.mark_math, "Math (leave blank for %d): ", 0, 100);
            reInputNum((unsigned long long *)&p->value.mark_eng, "English (leave blank for %d): ", 0, 100);
            reInputNum((unsigned long long *)&p->value.mark_c, "C Programming (leave blank for %d): ", 0, 100);
        }
        else if (*pCurrentList == stu_postgraduate)
        {
            reInputNum((unsigned long long *)&p->value.mark_overall, "Overall (leave blank for %d): ", 0, 100);
            reInputNum((unsigned long long *)&p->value.mark_paper, "Paper (leave blank for %d): ", 0, 100);
        }
        return 1;
    }
    return 0;
}

// generate id-value pairs of majors from database, output 2 lists to `int *dest` and `char **dest`
// args: `int *dest`, `char **dest`
int student_listMajorIds(node **pCurrentList, node *p, int index, va_list args)
{
    int *pListId = va_arg(args, int *);
    char **pListMajor = va_arg(args, char **);
    // match if major already in list
    int i;
    for (i = 0; pListMajor[i] != NULL && i < 1000; i++)
    {
        // major in list, skip appending
        if (strcmp(pListMajor[i], p->value.major) == 0)
        {
            return 0;
        }
    }
    // major not in list, append to the end
    if (i < 1000)
    {
        pListMajor[i] = p->value.major;
        // extract major id from student id (000001110000)
        pListId[i] = p->value.id / 10000 % 1000;
    }
    return 0;
}

// list students in a class, output list to `node **dest`
// args: `int classid`, `node **dest`
int student_listByClass(node **pCurrentList, node *p, int index, va_list args)
{
    if (p->value.classid == va_arg(args, int))
    {
        node **pList = va_arg(args, node **);
        // get list length
        int i;
        for (i = 0; pList[i] != NULL && i < 100; i++)
        {
        }
        // append to the end
        if (i < 100)
        {
            pList[i] = p;
        }
    }
    return 0;
}

int getMajorId(const char *major)
{
    int idList[1000] = {0};
    char *majorList[1000] = {NULL};
    students_forEach(student_listMajorIds, idList, majorList);
    // match if major already in database
    int i;
    for (i = 0; majorList[i] != NULL && i < 1000; i++)
    {
        if (strcmp(majorList[i], major) == 0)
        {
            return idList[i];
        }
    }
    // if no match, find the max major id then +1
    int max = 0;
    for (int i = 0; i < 1000; i++)
    {
        if (idList[i] > max)
        {
            max = idList[i];
        }
    }
    return max + 1;
}

int getClassStudentCount(int classid)
{
    node *studentList[100] = {NULL};
    students_forEach(student_listByClass, classid, studentList);
    // get list length
    int i;
    for (i = 0; studentList[i] != NULL && i < 100; i++)
    {
    }
    return i;
}

// generate student id, format: year[4] + type[1] + major[3] + class[2] + order[2]
unsigned long long generateStudentID(int type, char *major, int classid)
{
    // get current time
    time_t now = time(NULL);
    struct tm *time = localtime(&now);

    // 111123334400
    unsigned long long id = 100000000ULL * (time->tm_year + 1900) + 10000000ULL * type + 10000 * getMajorId(major) + classid * 100;
    return id + getClassStudentCount(classid) + 1;
}

#endif
