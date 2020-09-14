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

typedef struct lval
{
    int type;
    long num;
    int err;
} lval;
enum lval_type { LVAL_NUM, LVAL_ERR };
enum lval_err { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

lval lval_num(long x)
{
    lval val;
    val.type = LVAL_NUM;
    val.num = x;
    return val;
}

lval lval_err(int err)
{
    lval val;
    val.type = LVAL_ERR;
    val.err = err;
    return val;
}

void lval_print(lval val)
{
    switch (val.type)
    {
    case LVAL_NUM:
        printf("%li\n", val.num);
        break;
    case LVAL_ERR:

        switch (val.err)
        {
        case LERR_DIV_ZERO:
            fprintf(stderr, "Error: Division by zero!");
            break;
        case LERR_BAD_OP:
            fprintf(stderr, "Error: Invalid operator!");
            break;
        case LERR_BAD_NUM:
            fprintf(stderr, "Error: Invalid number!");
            break;
        default:
            break;
        }
    
    default:
        break;
    }
}

void lval_println(lval val)
{
    lval_print(val);
    putchar('\n');
}

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

lval eval_op(lval x, char *op, lval y)
{
    //TODO: add unit tests for this function

    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }

    fprintf(stderr, "Eval operator for: %li %s %li\n", x.num, op, y.num);
    if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
    if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
    if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
    if (strcmp(op, "/") == 0) { 
        return y.num == 0
        ? lval_err(LERR_DIV_ZERO)
        : lval_num(x.num / y.num);
    }
    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t *ast)
{
    //TODO: add unit tests for this function
    if (strstr(ast->tag, "number"))
    {
        fprintf(stderr, "Eval for: %s\n", ast->contents);
        errno = 0;
        long x = strtol(ast->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }
    
    char *operator = ast->children[1]->contents;
    fprintf(stderr, "Eval for: %s\n", operator);
    lval x = eval(ast->children[2]);

    int i = 3;
    while (strstr(ast->children[i]->tag, "expr"))
    {
        x = eval_op(x, operator, eval(ast->children[i]));
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

            lval value = eval(result.output);
            lval_println(value);

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
