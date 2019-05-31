#include "unit_test.h"
#include "../../src/helpers/parsing.h"
#include <string.h>

void is_whitespace_test();
void ignore_whitespaces_test();
void clean_token_test();

int main()
{
    is_whitespace_test();    
    ignore_whitespaces_test();
    clean_token_test();

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