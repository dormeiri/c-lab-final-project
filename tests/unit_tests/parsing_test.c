#include "unit_test.h"
#include "../../src/helpers/parsing.h"
#include <string.h>
#include <stdlib.h>

void is_whitespace_test();
void ignore_whitespaces_test();
void clean_token_test();
void map_statement_test(char *line, statement *expected);
void map_statement_error_test(char *line, errorCode code_expected);
void get_next_arg_test(char *args_str, address *expected);

int main()
{
    statement *expected = (statement *)malloc(sizeof(statement));
    address *a_expected = (address *)malloc(sizeof(address));

    is_whitespace_test();    
    ignore_whitespaces_test();
    clean_token_test();

    expected->tag = "TAG";
    expected->statement_type = OPERATION;
    expected->operation_type = MOV;
    expected->args = "#1,67,+42,-3,r5";
    map_statement_test("TAG:mov #1,67,+42,-3,r5", expected);
    
    a_expected->type = INSTANT;
    a_expected->value = 1;
    get_next_arg_test(expected->args, a_expected);

    a_expected->type = DATA;
    a_expected->value = 67;
    get_next_arg_test(NULL, a_expected);

    a_expected->type = DATA;
    a_expected->value = 42;
    get_next_arg_test(NULL, a_expected);    

    a_expected->type = DATA;
    a_expected->value = -3;
    get_next_arg_test(NULL, a_expected);    

    a_expected->type = REGISTER;
    a_expected->value = 5;
    get_next_arg_test(NULL, a_expected);    

    expected->tag = "TAG";
    expected->statement_type = DATA_KEY;
    expected->operation_type = NONE;
    expected->args = "a,b,c   \t ";
    map_statement_test("  \t TAG: \t .data \t  a,b,c   \t ", expected);

    expected->tag = NULL;
    expected->statement_type = STRING_KEY;
    expected->operation_type = NONE;
    expected->args = "a,b,c   \t ";
    map_statement_test("  \t .string \t  a,b,c   \t ", expected);

    expected->tag = NULL;
    expected->statement_type = OPERATION;
    expected->operation_type = JMP;
    expected->args = NULL;
    map_statement_test("jmp", expected);

    expected->tag = NULL;
    expected->statement_type = OPERATION;
    expected->operation_type = JMP;
    expected->args = NULL;
    map_statement_test(" \t   jmp       \t", expected);

    map_statement_error_test("  \t mov \t  a,b,c   \t ", OK);
    map_statement_error_test("  \t MOV: \t mov \t  a,b,c   \t ", OK);
    map_statement_error_test("  \t mov: \t mov \t  a,b,c   \t ", RESERVED_WORD);
    map_statement_error_test("  \t mo#v: \t mov \t  a,b,c   \t ", INVALID_TAG);
    map_statement_error_test("  \t 1ov: \t mov \t  a,b,c   \t ", INVALID_TAG);
    map_statement_error_test("  \t .1ov: \t mov \t  a,b,c   \t ", INVALID_TAG);

    return 0;
}

void is_whitespace_test()
{
    char *name = "is_whitespace_test";

    int actual;
    int expected;

    actual = IS_WHITESPACE(' ');
    expected = 1;

    assert(name, &actual, &expected, INT);

    actual = IS_WHITESPACE('a');
    expected = 0;

    assert(name, &actual, &expected, INT);
}

void ignore_whitespaces_test()
{
    char *name = "ignore_whitespace_test";

    char *actual;
    char *expected;

    actual = "                abc";
    expected = "abc";

    IGNORE_WHITE_SPACES(actual);

    assert(name, &actual, &expected, STRING);

    actual = "";
    expected = "";

    IGNORE_WHITE_SPACES(actual);

    assert(name, &actual, &expected, STRING);

    actual = " ";
    expected = "";

    IGNORE_WHITE_SPACES(actual);

    assert(name, &actual, &expected, STRING);

    actual = "a";
    expected = "a";

    IGNORE_WHITE_SPACES(actual);

    assert(name, &actual, &expected, STRING);
}

void clean_token_test()
{
    char *name = "clean_token_test";

    char str[BUFFER_SIZE] = "";
    char *actual;
    char *expected;

    actual = strncpy(str, "              fgdja            ", BUFFER_SIZE);
    expected = "fgdja";

    clean_token(&actual);

    assert(name, &actual, &expected, STRING);
}

void map_statement_test(char *line, statement *expected)
{
    static char *name = "map_statement";

    char *line_copy = (char *)malloc(strlen(line) * sizeof(char)); /* In case we got constant string */
    
    statement *actual = (statement *)malloc(sizeof(statement));

    strcpy(line_copy, line);

    map_statement(line_copy, actual);

    if(expected->tag)
    {
        assert(name, &(actual->tag), &(expected->tag), STRING);
    }
    else
    {
        assert(name, &(actual->tag), &(expected->tag), INT);
    }
    assert(name, &(actual->statement_type), &(expected->statement_type), INT);
    assert(name, &(actual->operation_type), &(expected->operation_type), INT);

    if(expected->args)
    {
        assert(name, &(actual->args), &(expected->args), STRING);
    }
    else
    {
        assert(name, &(actual->args), &(expected->args), INT);
    }
    
}

void map_statement_error_test(char *line, errorCode code_expected)
{

    static char *name = "map_statement";

    char *line_copy = (char *)malloc(strlen(line) * sizeof(char)); /* In case we got constant string */
    statement *temp;
    errorCode code_actual;

    strcpy(line_copy, line);
    code_actual = map_statement(line_copy, temp);

    assert(name, &code_actual, &code_expected, INT);
}

void get_next_arg_test(char *args_str, address *expected)
{
    char *name = "get_next_arg_test";
    address *actual = (address *)malloc(sizeof(address));
    char *args_str_copy;

    if(args_str == NULL)
    {
        get_next_arg(NULL, actual);
    }
    else
    {
        args_str_copy = (char *)malloc(strlen(args_str) * sizeof(char)); /* In case we got constant string */
        strcpy(args_str_copy, args_str);
        get_next_arg(args_str_copy, actual);
    }
    assert(name, &actual->type, &expected->type, INT);
    assert(name, &actual->value, &expected->value, INT);
}

void get_next_arg_error_test(char *args_str, errorCode expected)
{
    char *name = "get_next_arg_test";
    address *actual = (address *)malloc(sizeof(address));
    char *args_str_copy;
    errorCode res;

    if(args_str == NULL)
    {
        get_next_arg(NULL, actual);
    }
    else
    {
        args_str_copy = (char *)malloc(strlen(args_str) * sizeof(char)); /* In case we got constant string */
        strcpy(args_str_copy, args_str);
        get_next_arg(args_str_copy, actual);
    }
    res = get_next_arg(args_str_copy, actual);
    assert(name, &res, &expected, INT);
}