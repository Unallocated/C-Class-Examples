#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cmd {
    char *command;
} commands[] = {
    {
        "id"
    },
    {
        NULL
    }
};

int main(int argc, char *argv[])
{
    char buf[100];
    char *p;
    unsigned int i;

    while (!feof(stdin)) {
        printf("> ");

        if (!fgets(buf, sizeof(buf), stdin))
            break;

        p = strchr(buf, '\n');
        if (p != NULL)
            *p = '\0';

        p = strchr(buf, '#');
        if (p)
            *p = '\0';

        if (!strlen(buf))
            continue;

        if (!strcmp(buf, "quit") || !strcmp(buf, "exit"))
            break;

        for (i=0; commands[i].command != NULL; i++) {
            if (!strcmp(buf, commands[i].command)) {
                system(buf);
                break;
            }
        }

        if (commands[i].command == NULL)
            fprintf(stderr, "Error: Command not found or not allowed\n");
    }

    return 0;
}
