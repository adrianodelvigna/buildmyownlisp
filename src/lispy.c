#include <stdio.h>
#include <stdlib.h>
#include "version.h"

#ifdef _WIN32
#include <string.h>

const unsigned int BufferLength = 2048;
static char buffer[BufferLength];

char *readline(char *prompt)
{
    fputs(prompt, stdout);
    fgets(buffer, BufferLength, stdin);

    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';

    return cpy;
}

void add_history(char *unused) {}

#else
#include <editline/readline.h>
#endif

int main(int argc, char const *argv[])
{
    printf("Lispy version %d.%d.%d\n", Lispy_VERSION_MAJOR, Lispy_VERSION_MINOR, Lispy_VERSION_PATCH);
    puts("Press Ctrl+c to exit\n");

    while (1)
    {
        char *input = readline("lispy> ");
        add_history(input);
        printf("You are a %s\n", input);
        free(input);
        input = NULL;
    }

    return 0;
}
