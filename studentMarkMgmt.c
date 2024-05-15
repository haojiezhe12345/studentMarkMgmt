#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "linkedList.h"
#include "inputs.h"
#include "student.h"

#define db_undergraduate "db_undergraduate.bin"
#define db_postgraduate "db_postgraduate.bin"

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
            printf("ls, list          List students\n");
            printf("add               Add student\n");
            printf("mod, modify       Modify student\n");
            printf("del, delete       Delete student\n");
            printf("mark, marks       Marks management\n");
            printf("save              Save databases to file\n");
            printf("load              Load databases from file\n");
            printf("q, quit, exit     Exit program\n");
        }

        // student list
        else if (strcmp(cmd, "ls") == 0 || strcmp(cmd, "list") == 0)
        {
            students_forEach(student_print);
        }

        // student add
        else if (strcmp(cmd, "add") == 0)
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
        else if (strcmp(cmd, "mod") == 0 || strcmp(cmd, "modify") == 0)
        {
            unsigned long long id;
            if (sscanf(cmds, "%s %llu", cmd, &id) == 2)
            {
                if (students_forEach(student_modify, id))
                {
                    printf("Student not found\n");
                }
            }
            else
            {
                printf("Usage: modify <Student ID>\n");
            }
        }

        // student delete
        else if (strcmp(cmd, "del") == 0 || strcmp(cmd, "delete") == 0)
        {
            unsigned long long id;
            if (sscanf(cmds, "%s %llu", cmd, &id) == 2)
            {
                if (students_forEach(student_delete_confirm, id))
                {
                    printf("Student not found\n");
                }
            }
            else
            {
                printf("Usage: delete <Student ID>\n");
            }
        }

        // marks management
        else if (strcmp(cmd, "mark") == 0 || strcmp(cmd, "marks") == 0)
        {
            char subcmd[64];
            unsigned long long id;
            int params = sscanf(cmds, "%s %s %llu", cmd, subcmd, &id);
            if (params == 3 && strcmp(subcmd, "show") == 0)
            {
                if (students_forEach(student_marks_show, id))
                {
                    printf("Student not found\n");
                }
            }
            else if (params == 3 && strcmp(subcmd, "update") == 0)
            {
                if (students_forEach(student_marks_update, id))
                {
                    printf("Student not found\n");
                }
            }
            else
            {
                printf("marks show <Student ID>      Show student's marks\n");
                printf("marks update <Student ID>    Update student's marks\n");
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