#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#include "linkedList.h"
#include "inputs.h"
#include "students.h"

// db location
#define db_undergraduate "db_undergraduate.bin"
#define db_postgraduate "db_postgraduate.bin"

char allcmds[][16] = {
    "help",
    "print",
    "ls",
    "list",
    "add",
    "edit",
    "delete",
    "show",
    "rank",
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

// returns non-zero on `exit` command
int runCmd(const char *cmds, const char *cmd)
{
    if (strcmp(cmd, "help") == 0)
    {
        printf("help          Show help\n");
        printf("print         Print all students\n");
        printf("ls, list      List students (with paging, sorting & filtering)\n");
        printf("add           Add student\n");
        printf("edit          Edit student info\n");
        printf("delete        Delete student\n");
        printf("show          Show a student's info\n");
        printf("rank          Show a student's marks and ranks (aka 'marks show')\n");
        printf("marks         Marks management\n");
        printf("save          Save databases to file\n");
        printf("load          Load databases from file\n");
        printf("quit, exit    Exit program\n");
        printf("\nNote: Short-handed commands are accepted\nFor example, \"a\" -> \"add\", \"d\" or \"del\" -> \"delete\"\n");
    }

    // student print
    else if (strcmp(cmd, "print") == 0)
    {
        students_forEach(student_print);
    }

    // student list
    else if (strcmp(cmd, "ls") == 0 || strcmp(cmd, "list") == 0)
    {
        char subcmd[64];
        // parse params
        int paramsCount = sscanf(cmds, "%*s %s", subcmd);
        // complete sub command
        char subcmds[][16] = {
            "undergraduate",
            "postgraduate",
        };
        strcpy(subcmd, matchCmd(subcmds, sizeof(subcmds) / sizeof(subcmds[0]), subcmd));

        node *head;
        if (paramsCount == 1 && strcmp(subcmd, "undergraduate") == 0)
        {
            head = stu_undergraduate;
        }
        else if (paramsCount == 1 && strcmp(subcmd, "postgraduate") == 0)
        {
            head = stu_postgraduate;
        }
        else
        {
            printf("ls undergraduate    List undergraduate students\n");
            printf("ls postgraduate     List postgraduate students\n");
            return 0;
        }

        // arrow keys to browse pages
        node **nodes;
        int size = 10;
        int page = 0;
        // sort options
        int *sortBy = NULL;
        int sortOrder = 1;

        // calculate marks
        students_forEach(student_calc_totalmarks);
        // clone list for viewing, sorting & filtering, without affecting the original
        node* viewList = cloneList(head);

        while (1)
        {
            // get paging info
            int total = getNodesCount(viewList);
            if (page < 0)
            {
                page = 0;
            }
            if (page * size + 1 > total)
            {
                page = (total - 1) / size;
            }
            printf("\nDisplaying %d - %d of %d\n\n", page * size + 1, (page + 1) * size, total);

            // get nodes by page
            nodes = getPagedNodes(viewList, size, page);
            // print nodes
            printf("Student ID     Name                 Gender   Major        Class       Marks\n");
            printf("---------------------------------------------------------------------------\n");
            for (int i = 0; i < size && nodes[i] != NULL; i++)
            {
                node *p = nodes[i];
                student_print_with_totalmarks(NULL, p, 0, NULL);
            }
            printf("\nArrow keys: Navigate    Home/End: First/Last Page    z: Page size");
            printf("\ns: Sort by    o: Sort order    f: Filter    q/Ctrl-C: Quit");

            // detect keyboard press
            int ch = getch();
            if (ch == 0 || ch == 224) // prefix for arrow keys
            {
                switch (getch()) // the actual key code
                {
                case 72: // up
                    page--;
                    break;
                case 80: // down
                    page++;
                    break;
                case 75: // left
                    page--;
                    break;
                case 77: // right
                    page++;
                    break;
                case 71: // home
                    page = 0;
                    break;
                case 79: // end
                    page = (total - 1) / size;
                    break;
                case 73: // page up
                    page--;
                    break;
                case 81: // page down
                    page++;
                    break;
                }
            }
            // page size
            else if (ch == 'z')
            {
                printf("\n\n");
                reInputInt(&size, "Input page size (leave blank for %d): ", 0, 1000);
            }
            // sorting
            else if (ch == 's' || ch == 'o')
            {
                if (ch == 's')
                {
                    printf("\n\nSort by:\n1. Student ID\n2. Class\n3. Total marks\n");
                    int order = inputSelect("Choose a sort key: ", 3);
                    switch (order)
                    {
                    case 1:
                        // WARNING: converting int64 to int32 may cause incorrect sort!
                        // this will cause sorting regards only the first 4 bytes of an 8-byte integer,
                        // in little-endian systems, they represent lower digits, while the big-endian's represent higher
                        sortBy = (int *)&viewList->value.id;
                        break;
                    case 2:
                        sortBy = &viewList->value.classid;
                        break;
                    case 3:
                        sortBy = &viewList->value.totalmarks;
                        sortOrder = 2;
                        break;
                    }
                }
                if (ch == 'o')
                {
                    sortOrder = sortOrder == 1 ? 2 : 1;
                }
                if (sortBy)
                {
                    bubbleSortByIntValue(&viewList, sortBy, sortOrder);
                }
            }
            // free the `viewList` and quit
            else if (ch == 'q' || ch == 3)
            {
                printf("\n");
                destroyList(&viewList);
                return 0;
            }
            printf("\n");
        }
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
        if (sscanf(cmds, "%*s %llu", &id) == 1)
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
        if (sscanf(cmds, "%*s %llu", &id) == 1)
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

    // student show
    else if (strcmp(cmd, "show") == 0)
    {
        unsigned long long id;
        char name[32];
        node *p;
        if (sscanf(cmds, "%*s %llu", &id) == 1)
        {
            if (students_forEach(student_getNodeById, id, &p))
            {
                printf("Student not found\n");
                return 0;
            }
        }
        else if (sscanf(cmds, "%*s %s", name) == 1)
        {
            if (students_forEach(student_getNodeByName, name, &p))
            {
                printf("Student not found\n");
                return 0;
            }
        }
        else
        {
            printf("Usage: show <Name | Student ID>\n");
            return 0;
        }

        printf("Basic info:\n");
        printf("Type   : %s\n", p->value.id / 10000000 % 10 == 1 ? "Undergraduate" : "Postgraduate");
        printf("ID     : %llu\n", p->value.id);
        printf("Name   : %s\n", p->value.name);
        printf("Gender : %s\n", p->value.gender == 1 ? "Male" : "Female");
        printf("Major  : %s\n", p->value.major);
        printf("Class  : %d\n", p->value.classid);

        printf("\nMarks:\n");
        if (p->value.id / 10000000 % 10 == 1)
        {
            printf("Math:          %d\n", p->value.mark_math);
            printf("English:       %d\n", p->value.mark_eng);
            printf("C Programming: %d\n", p->value.mark_c);
        }
        else
        {
            printf("Overall: %d\n", p->value.mark_overall);
            printf("Paper:   %d\n", p->value.mark_paper);
        }
    }

    // show marks and ranks
    else if (strcmp(cmd, "rank") == 0)
    {
        unsigned long long id;
        if (sscanf(cmds, "%*s %llu", &id) == 1)
        {
            if (students_forEach(student_marks_show, id))
            {
                printf("Student not found\n");
            }
        }
        else
        {
            printf("Usage: rank <Student ID>\n");
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
        return 1;
    }

    else
    {
        printf("Command not recognized\n");
    }

    return 0;
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
        char *cmds = inputStr("> ", 256);

        if (cmds[0] == '\0')
        {
            continue;
        }

        char cmd[256];
        sscanf(cmds, "%s", cmd);
        // complete the command
        strcpy(cmd, matchCmd(allcmds, sizeof(allcmds) / sizeof(allcmds[0]), cmd));

        if (runCmd(cmds, cmd))
            break;

        printf("\n");
    }
}