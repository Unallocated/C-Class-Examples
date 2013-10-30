#include <stdio.h>

int main(int argc, char *argv[])
{
    char hello[] = "Hello, World!\n";
    unsigned int i;

    for (i=0; i < sizeof(hello); i++)
        putc((int)hello[i], stdout);

    i=0;
    while (hello[i] != '\0')
        putc((int)hello[i++], stdout);

    return 0;
}
