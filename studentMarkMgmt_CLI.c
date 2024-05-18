#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "linkedList.h"
#include "inputs.h"
#include "student.h"

// db location
#define db_undergraduate "db_undergraduate.bin"
#define db_postgraduate "db_postgraduate.bin"

char allcmds[][16] = {
    "help",
    "ls",
    "list",
    "add",
    "edit",
    "delete",
    "marks",
    "save",
    "load",
    "exit",
    "quit",
};

// match and complete a command
// returns the completed command if there's only one match
// for example: h->help, a->add
char *matchCmd(char cmdList[][16], int listSize, char *cmd)
{
    int matchedCmds = 0;
    int index;
    // loop over commands and find how many commands are matched
    for (int i = 0; i < listSize; i++)
    {
        // compare each char
        for (int j = 0; j < strlen(cmd) && j < strlen(cmdList[i]); j++)
        {
            // break on char not match
            if (cmd[j] != cmdList[i][j])
            {
                break;
            }
            // if successfully looped to the last char, then it's fully matched
            if (j == strlen(cmd) - 1)
            {
                matchedCmds++;
                index = i;
            }
        }
    }

    if (matchedCmds == 1)
    {
        return cmdList[index];
    }
    else
    {
        return cmd;
    }
}

int main()
{
    printf("\n=======================================");
    printf("\n Student Marks Management System - CLI ");
    printf("\n=======================================\n");

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
        // complete the command
        strcpy(cmd, matchCmd(allcmds, sizeof(allcmds) / sizeof(allcmds[0]), cmd));

        if (strcmp(cmd, "help") == 0)
        {
            printf("help          Show help\n");
            printf("ls, list      List students\n");
            printf("add           Add student\n");
            printf("edit          Edit student info\n");
            printf("delete        Delete student\n");
            printf("marks         Marks management\n");
            printf("save          Save databases to file\n");
            printf("load          Load databases from file\n");
            printf("quit, exit    Exit program\n");
            printf("\nNote: Short-handed commands are accepted\nFor example, \"a\" -> \"add\", \"d\" or \"del\" -> \"delete\"\n");
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

            // generate id
            stu.id = generateStudentID(stuType, stu.major, stu.classid);

            // init marks
            stu.mark_math = -1;
            stu.mark_eng = -1;
            stu.mark_c = -1;
            stu.mark_overall = -1;
            stu.mark_paper = -1;
            stu.totalmarks = -1;
            stu.rank_school = -1;
            stu.rank_class = -1;

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

        // student edit
        else if (strcmp(cmd, "edit") == 0)
        {
            unsigned long long id;
            if (sscanf(cmds, "%s %llu", cmd, &id) == 2)
            {
                if (students_forEach(student_edit, id))
                {
                    printf("Student not found\n");
                }
            }
            else
            {
                printf("Usage: edit <Student ID>\n");
            }
        }

        // student delete
        else if (strcmp(cmd, "delete") == 0)
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
        else if (strcmp(cmd, "marks") == 0)
        {
            char subcmd[64];
            unsigned long long id;
            // parse params
            int paramsCount = sscanf(cmds, "%*s %s %llu", subcmd, &id);
            // complete sub command
            char subcmds[][16] = {
                "show",
                "update",
            };
            strcpy(subcmd, matchCmd(subcmds, sizeof(subcmds) / sizeof(subcmds[0]), subcmd));

            if (paramsCount == 2 && strcmp(subcmd, "show") == 0)
            {
                if (students_forEach(student_marks_show, id))
                {
                    printf("Student not found\n");
                }
            }
            else if (paramsCount == 2 && strcmp(subcmd, "update") == 0)
            {
                if (students_forEach(student_marks_update, id))
                {
                    printf("Student not found\n");
                }
            }
            else
            {
                printf("marks show <Student ID>      Show student's marks and ranks\n");
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

        else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0)
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