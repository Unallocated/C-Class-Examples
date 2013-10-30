#include <stdio.h>

struct hello {
    const char *message;
};

struct hello *get_hello(void)
{
    struct hello *hello;

    hello = calloc(1, sizeof(struct hello));
    if (!(hello))
        return NULL;

    hello->message = "Hello, World!";

    return hello;
}

int main(int argc, char *argv[])
{
    struct hello *hello;

    hello = get_hello();
    if (!(hello)) {
        fprintf(stderr, "ALL ABOARD THE FAILBOAT!\n");
        return 1;
    }

    printf("%s\n", hello->message);

    free(hello);

    return 0;
}
