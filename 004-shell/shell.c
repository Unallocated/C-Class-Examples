#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFSZ 1024

int do_cd(char **);
int do_getenv(char **);
int do_setenv(char **);

typedef struct _cmd {
        char *name;
        char *usage;
        int minArgs;
        int (*handler)(char **);
} CMD;

CMD cmds[] = {
        {
                "cd",
                "cd <directory>",
                1,
                do_cd
        },
        {
                "getenv",
                "getenv VARIABLE",
                1,
                do_getenv
        },
        {
                "setenv",
                "setenv VARIABLE \"data\"",
                2,
                do_setenv
        },
        {
                NULL,
                NULL,
                0,
                NULL
        }
};

int readf(char *buf, unsigned int bufsz, FILE *fp)
{
        unsigned int i=0;
        
        while (i < bufsz-1)
                if ((buf[i] = getc(fp)) == EOF
                        || buf[i++] == '\n')
                                break;
        
        if (buf[i] == EOF)
                buf[i] = 0x00;
        else if (buf[i-1] == '\n')
                buf[i-1] = 0x00;
        
        return i;
}

char **parse_line(char *line, unsigned int linsz)
{
        char *p1, *p2;
        unsigned int entry=0;
        char **retval=NULL;
        
        p1 = line;
        while (*p1 && p1 - line < linsz)
        {
                switch (*p1)
                {
                        case '\"':
                                p2 = strchr(++p1, '\"');
                                if (!p2)
                                        return NULL;
                                *p2 = 0x00;
                                break;
                        default:
                                p2 = strchr(p1, ' ');
                                if (p2)
                                        *p2 = 0x00;
                                else
                                        p2 = p1+strlen(p1);
                                break;
                }
                
                retval = realloc(retval, (entry + 1) * sizeof(char **));
                if (!retval)
                        return NULL;
                retval[entry++] = p1;
                p1 = p2+1;
        }
        
        retval = realloc(retval, (entry + 1) * sizeof(char **));
        if (!retval)
                return NULL;
        retval[entry] = NULL;
        
        return retval;
}

void sig_handler(int signo)
{
        switch (signo)
        {
                case SIGCHLD:
                        waitpid(0, NULL, 0);
                        break;
                default:
                        break;
        }
}

int main(int argc, char *argv[], char **envp)
{
        char buf[BUFSZ+1];
        char **prog;
        int pid, status, runExec;
        unsigned int i, j;
        
        if (argc > 1)
                execvp(argv[1], &argv[1]);
        
        signal(SIGCHLD, sig_handler);
        
        while (!feof(stdin))
        {
                getcwd(buf, BUFSZ);
                printf("%s %s", buf, ((!getuid()) ? "# ": "$ "));
                
                memset(buf, 0x00, BUFSZ);
                readf(buf, BUFSZ, stdin);
                prog = parse_line(buf, strlen(buf));
                
                if (!prog)
                {
                        fprintf(stderr, "Syntax error\n");
                        exit(0);
                }
                
                if (prog[0] == NULL || prog[0][0] == 0x00)
                        continue;
                
                runExec = 1;
                
                for (i=0; cmds[i].name != NULL; i++)
                {
                        if (!strcmp(prog[0], cmds[i].name))
                        {
                                /* Make sure minimum arguments are satisfied */
                                for (j=1; prog[j] != NULL; j++)
                                ;
                                
                                if (j > cmds[i].minArgs)
                                        cmds[i].handler(prog);
                                else
                                        fprintf(stderr, "%s\n", cmds[i].usage);
                                
                                runExec = 0;
                                break;
                        }
                }
                
                if (!runExec)
                        continue;
                
                if (!strcmp(prog[0], "quit") || !strcmp(prog[0], "exit"))
                        exit(0);
                
                /* Run in background (reusing the runExec variable to check if we should run in background) */
                runExec = i = 0;
                while (prog[i+1] != NULL)
                        i++;
                
                if (strlen(prog[i]) == 1 && prog[i][0] == '&')
                {
                        runExec = 1;
                        prog[i] = NULL;
                }
                
                switch ((pid = fork()))
                {
                        case 0:
                                execvp(prog[0], prog);
                                perror("exec");
                                exit(1);
                                break;
                        case -1:
                                break;
                        default:
                                //free(prog);
                                waitpid(pid, &status, (runExec) ? WNOHANG : 0);
                                break;
                }
        }
        
        return 0;
}

int do_cd(char **prog)
{
        return chdir(prog[1]);
}

int do_getenv(char **prog)
{
        char *p1;
        
        if (prog[1] == NULL)
        {
                fprintf(stderr, "USAGE: getenv VARIABLE\n");
                return -1;
        }
        if ((p1 = getenv(prog[1])))
                printf("%s = \"%s\"\n", prog[1], p1);
        else
                fprintf(stderr, "%s does not exist\n", prog[1]);
        
        return 0;
}

int do_setenv(char **prog)
{
        if (prog[1] == NULL)
        {
                fprintf(stderr, "USAGE: setenv VARIABLE \"data\"\n");
                return -1;
        }
        
        return setenv(prog[1], prog[2], 1);
}