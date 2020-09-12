#include <stdio.h>
#include <stdlib.h>
#include "version.h"
#include "mpc/mpc.h"

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
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Operator = mpc_new("operator");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
        "number     : /-?[0-9]+/ ;"
        "operator   : '+' | '-' | '*' | '/' ;"
        "expr       : <number> | '(' <operator> <expr>+ ')' ;"
        "lispy      : /^/ <operator> <expr>+ /$/ ;"
    , Number, Operator, Expr, Lispy);

    printf("Lispy version %d.%d.%d\n", Lispy_VERSION_MAJOR, Lispy_VERSION_MINOR, Lispy_VERSION_PATCH);
    puts("Press Ctrl+c to exit\n");

    while (1)
    {
        char *input = readline("lispy> ");
        add_history(input);

        mpc_result_t result;
        if (mpc_parse("<stdin>", input, Lispy, &result))
        {
            mpc_ast_print(result.output);
            mpc_ast_delete(result.output);
        }
        else 
        {
            mpc_err_print(result.error);
            mpc_err_delete(result.error);
        }

        free(input);
        input = NULL;
    }

    mpc_cleanup(4, Number, Operator, Expr, Lispy);

    return 0;
}
