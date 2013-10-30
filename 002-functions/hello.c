#include <stdio.h>

const char *get_hello(void)
{
    return "Hello, World!";
}

void print_hello(const char *hello)
{
    printf("%s\n", hello);
    printf(hello); /* This is really, really bad */
    putc('\n', stdout);
}

int main(int argc, char *argv[])
{
    const char *hello;

    hello = get_hello();
    print_hello(hello);
    print_hello(get_hello());

    return 0;
}
