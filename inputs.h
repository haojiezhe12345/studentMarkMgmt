#ifndef inputs
#define inputs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void reInputStr(char *target, const char *msg, int len)
{
    printf(msg, target);
    char *str = inputStr("", len);
    if (*str != '\0')
    {
        strcpy(target, str);
    }
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

void reInputNum(unsigned long long *target, const char *msg, unsigned long long min, unsigned long long max)
{
    unsigned long long num;
    while (1)
    {
        printf(msg, *target);
        char *str = inputStr("", 32);
        if (*str != '\0')
        {
            sscanf(str, "%llu", &num);
            if ((min <= num && num <= max))
            {
                *target = num;
                break;
            }
            else
            {
                printf("Value should be between %llu and %llu!\n", min, max);
            }
        }
        else
        {
            break;
        }
    }
}

// input asking for a selection (max 9 choices)
int inputSelect(const char *msg, int range)
{
    char *ch;
    while (1)
    {
        ch = inputStr(msg, 32);
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

void reInputSelect(int *target, const char *msg, int range)
{
    char *ch;
    while (1)
    {
        printf(msg, *target);
        ch = inputStr("", 32);
        if ('1' <= *ch && *ch <= '0' + range)
        {
            *target = *ch - '0';
            break;
        }
        // knwon issue: entering even length of string causes the last input to be `\0`, which leaves this unchanged
        else if (*ch == '\0')
        {
            break;
        }
        else
        {
            printf("Invalid choice!\n");
        }
    }
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

#endif
