/*
 * screen.c - terminal helpers
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"

#ifndef APP_VERSION
#define APP_VERSION "2.4.0"
#endif

void cls(void)
{
    printf("\033[2J\033[H");
}

void header(const char *title)
{
    char ver[32];
    cls();
    if (APP_VERSION[0] == 'v' || APP_VERSION[0] == 'V')
        snprintf(ver, sizeof ver, "  DEALERDESK %s", APP_VERSION);
    else
        snprintf(ver, sizeof ver, "  DEALERDESK v%s", APP_VERSION);

    printf("==============================================================\n");
    printf("%-30s %31s\n", ver, title);
    printf("==============================================================\n\n");
}

void read_line(const char *prompt, char *buf, int len)
{
    char *p;
    int c;

    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, len, stdin) == NULL) {
        printf("\n");
        exit(0);
    }
    p = strchr(buf, '\n');
    if (p)
        *p = 0;
    else
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    /* a | would break the data files */
    for (p = buf; *p; p++)
        if (*p == '|')
            *p = '/';
}

int read_int(const char *prompt)
{
    char b[32];
    read_line(prompt, b, sizeof b);
    return atoi(b);
}

double read_money(const char *prompt)
{
    char b[32];
    read_line(prompt, b, sizeof b);
    return atof(b);
}

void pause_enter(void)
{
    char b[8];
    read_line("\n  Press ENTER to continue...", b, sizeof b);
}

int confirm(const char *prompt)
{
    char b[8];
    read_line(prompt, b, sizeof b);
    return b[0] == 'Y' || b[0] == 'y';
}
