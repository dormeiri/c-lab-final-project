#include "unit_test.h"
#include "../../src/helpers/parsing.h"
#include <string.h>
#include <stdlib.h>

void is_whitespace_test();
void ignore_whitespaces_test();
void clean_token_test();
void split_statement_test(char *line, char *tag_expected, char *statement_key_expected, char *args_expected);
void split_statement_error_test(char *line, errorCode code_expected);

int main()
{
    is_whitespace_test();    
    ignore_whitespaces_test();
    clean_token_test();

    split_statement_test("TAG:mov a,b,c", "TAG", "mov", "a,b,c");
    split_statement_test("  \t TAG: \t mov \t  a,b,c   \t ", "TAG", "mov", "\t  a,b,c   \t ");
    split_statement_test("  \t mov \t  a,b,c   \t ", NULL, "mov", "\t  a,b,c   \t ");

    split_statement_error_test("  \t mov \t  a,b,c   \t ", OK);
    split_statement_error_test("  \t MOV: \t mov \t  a,b,c   \t ", OK);
    split_statement_error_test("  \t mov: \t mov \t  a,b,c   \t ", RESERVED_WORD);
    split_statement_error_test("  \t mo#v: \t mov \t  a,b,c   \t ", INVALID_TAG);
    split_statement_error_test("  \t 1ov: \t mov \t  a,b,c   \t ", INVALID_TAG);
    split_statement_error_test("  \t .1ov: \t mov \t  a,b,c   \t ", INVALID_TAG);

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

void split_statement_test(char *line, char *tag_expected, char *statement_key_expected, char *args_expected)
{
    static char *name = "split_statement";

    char *line_copy = (char *)malloc(strlen(line) * sizeof(char)); /* In case we got constant string */
    char *tag_actual;
    char *statement_key_actual;
    char *args_actual;

    strcpy(line_copy, line);

    split_statement(line_copy, &tag_actual, &statement_key_actual, &args_actual);

    if(tag_expected)
    {
        assert(name, &tag_actual, &tag_expected, STRING);
    }
    else
    {
        assert(name, &tag_actual, &tag_expected, INT);
    }
    assert(name, &statement_key_actual, &statement_key_expected, STRING);
    assert(name, &args_actual, &args_expected, STRING);
}

void split_statement_error_test(char *line, errorCode code_expected)
{

    static char *name = "split_statement";

    char *line_copy = (char *)malloc(strlen(line) * sizeof(char)); /* In case we got constant string */
    char *tag_actual;
    char *statement_key_actual;
    char *args_actual;
    errorCode code_actual;

    strcpy(line_copy, line);
    code_actual = split_statement(line_copy, &tag_actual, &statement_key_actual, &args_actual);

    assert(name, &code_actual, &code_expected, INT);
}