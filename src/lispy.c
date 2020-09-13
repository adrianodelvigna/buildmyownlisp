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

int number_of_nodes(mpc_ast_t *ast)
{
    //TODO: add unit tests for this function
    if (ast->children_num == 0) { return 1; }
    if (ast->children_num >= 1)
    {
        int total = 1;
        for (int i = 0; i < ast->children_num; i++)
        {
            total += number_of_nodes(ast->children[i]);
        }
        return total;
    }
    return 0;
}

#define OPERATORS   \
    OPERATOR(+)     \
    OPERATOR(-)     \
    OPERATOR(*)     \
    OPERATOR(/)

long eval_op_x_macros(long x, char *op, long y)
{
    //TODO: add unit tests for this function
    fprintf(stderr, "Eval operator for: %li %s %li (*)\n", x, op, y);
    #define OPERATOR(o) if (strcmp(op, #o) == 0) { return x o y; }
    OPERATORS
    #undef OP
    return 0;
}

long eval_op(long x, char *op, long y)
{
    //TODO: add unit tests for this function
    fprintf(stderr, "Eval operator for: %li %s %li\n", x, op, y);
    if (strcmp(op, "+") == 0) { return x + y; }
    if (strcmp(op, "-") == 0) { return x - y; }
    if (strcmp(op, "*") == 0) { return x * y; }
    if (strcmp(op, "/") == 0) { return x / y; }
    return 0;
}

long eval(mpc_ast_t *ast)
{
    //TODO: add unit tests for this function
    fprintf(stderr, "Eval for: %s\n", ast->contents);
    if (strstr(ast->tag, "number"))
    {
        return atoi(ast->contents);
    }
    
    char *operator = ast->children[1]->contents;
    long x = eval(ast->children[2]);

    int i = 3;
    while (strstr(ast->children[i]->tag, "expr"))
    {
        x = eval_op_x_macros(x, operator, eval(ast->children[i]));
        i++;
    }
    
    return x;
}

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
            printf("Number of nodes: %d\n", number_of_nodes(result.output));
            mpc_ast_print(result.output);

            long value = eval(result.output);
            printf("\n-> %li\n", value);

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
