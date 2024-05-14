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

#endif
