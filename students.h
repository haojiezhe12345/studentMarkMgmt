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
// returns 1 if looped completely, 0 if aborted
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

// find student node by student id, output to `node **dest`
// args: `unsigned long long id`, `node **dest`
int student_getNodeById(node **pCurrentList, node *p, int index, va_list args)
{
    if (p->value.id == va_arg(args, unsigned long long))
    {
        node **dest = va_arg(args, node **);
        *dest = p;
        return 1;
    }
    return 0;
}

int student_getNodeByName(node **pCurrentList, node *p, int index, va_list args)
{
    if (strcmp(p->value.name, va_arg(args, char *)) == 0)
    {
        node **dest = va_arg(args, node **);
        *dest = p;
        return 1;
    }
    return 0;
}

int student_print(node **pCurrentList, node *p, int index, va_list args)
{
    printf("%-14llu %-20s %-8s %-12s Class %-5d\n", p->value.id, p->value.name, p->value.gender == 1 ? "Male" : "Female", p->value.major, p->value.classid);
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

int student_edit(node **pCurrentList, node *p, int index, va_list args)
{
    if (p->value.id == va_arg(args, unsigned long long))
    {
        reInputStr(p->value.name, "Name (leave blank for %s): ", 32);
        reInputSelect(&p->value.gender, "Gender (1=Male, 2=Female, leave blank for %d): ", 2);
        reInputStr(p->value.major, "Major (leave blank for %s): ", 64);
        reInputInt(&p->value.classid, "Class (leave blank for %d): ", 1, 99);

        if (*pCurrentList == stu_postgraduate)
        {
            reInputStr(p->value.direction, "Research direction (leave blank for %s): ", 64);
            reInputStr(p->value.tutor, "Tutor (leave blank for %s): ", 32);
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
            reInputInt(&p->value.mark_math, "Math (leave blank for %d): ", 0, 100);
            reInputInt(&p->value.mark_eng, "English (leave blank for %d): ", 0, 100);
            reInputInt(&p->value.mark_c, "C Programming (leave blank for %d): ", 0, 100);
        }
        else if (*pCurrentList == stu_postgraduate)
        {
            reInputInt(&p->value.mark_overall, "Overall (leave blank for %d): ", 0, 100);
            reInputInt(&p->value.mark_paper, "Paper (leave blank for %d): ", 0, 100);
        }
        return 1;
    }
    return 0;
}

int student_calc_totalmarks(node **pCurrentList, node *p, int index, va_list args)
{
    if (*pCurrentList == stu_undergraduate)
    {
        if (p->value.mark_math >= 0 && p->value.mark_eng >= 0 && p->value.mark_c >= 0)
        {
            p->value.totalmarks = p->value.mark_math + p->value.mark_eng + p->value.mark_c;
        }
        else
        {
            p->value.totalmarks = -1;
        }
    }
    else if (*pCurrentList == stu_postgraduate)
    {
        if (p->value.mark_overall >= 0 && p->value.mark_paper >= 0)
        {
            p->value.totalmarks = p->value.mark_overall + p->value.mark_paper;
        }
        else
        {
            p->value.totalmarks = -1;
        }
    }
    return 0;
}

// calculate rank, save result to student node
// args: `node *pRank`
int student_marks_rank(node **pCurrentList, node *p, int index, va_list args)
{
    node *pRank = va_arg(args, node *);

    // if ranking starts, calculate total marks and set rank to 1
    if (p == stu_undergraduate)
    {
        students_forEach(student_calc_totalmarks);
        // abort if marks are not valid
        if (pRank->value.totalmarks < 0)
        {
            return 1;
        }
        // init rank to 1
        pRank->value.rank_school = 1;
        pRank->value.rank_class = 1;
    }

    if (p->value.id / 10000000 % 10 == pRank->value.id / 10000000 % 10 && // match type
        p->value.totalmarks > pRank->value.totalmarks)                    // greater
    {
        // found greater marks, rank +1
        pRank->value.rank_school++;
        // check if is same class
        if (p->value.id / 100000000ULL == pRank->value.id / 100000000ULL && // year
            strcmp(p->value.major, pRank->value.major) == 0 &&              // major
            p->value.classid == pRank->value.classid)                       // class
        {
            pRank->value.rank_class++;
        }
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
        students_forEach(student_marks_rank, p);
        printf("Total marks: %d (School rank: %d, Class rank: %d)\n", p->value.totalmarks, p->value.rank_school, p->value.rank_class);
        return 1;
    }
    return 0;
}

// generate id-value pairs of majors from database, output 2 lists to `int *dest` and `char **dest`
// major id is extracted from student id (000001110000) at first occurrence of its major
// for example: `202412330101 "major1"` -> major1 = 233
// args: `int *dest`, `char **dest`
int student_listMajorIds(node **pCurrentList, node *p, int index, va_list args)
{
    int *pListId = va_arg(args, int *);
    char **pListMajor = va_arg(args, char **);
    // match if major already in list
    int i;
    for (i = 0; pListMajor[i] != NULL && i < 1000; i++)
    {
        // major already in list, skip appending
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

// list all students in the class that a student belongs to, identified by student id 111111111100
// output list to `node **dest`
// args: `unsigned long long studentId`, `node **dest`
int student_listClassStudentsByStudentId(node **pCurrentList, node *p, int index, va_list args)
{
    // match student id: 111111111100
    if (p->value.id / 100 == va_arg(args, unsigned long long) / 100)
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

// calculate major id from database rows, returns max + 1 if not in database
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

// get number of students in the class that the student belongs to, identified by student id 111111111100
int getClassStudentCountByStudentId(unsigned long long id)
{
    node *studentList[100] = {NULL};
    students_forEach(student_listClassStudentsByStudentId, id, studentList);
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

    // generate 111123334400
    unsigned long long id = 100000000ULL * (time->tm_year + 1900) + 10000000ULL * type + 10000 * getMajorId(major) + classid * 100;
    // generate 000000000011
    return id + getClassStudentCountByStudentId(id) + 1;
}

#endif
