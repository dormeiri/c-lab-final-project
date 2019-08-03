#include "unit_test.h"
#include "../../src/helpers/parsing.h"
#include "../../src/helpers/validations.h"

void check_cleaned_token_test();

int main()
{
    check_cleaned_token_test();

    return 0;
}


void check_cleaned_token_test()
{
    char *name = "check_cleaned_token_test";

    char str[BUFFER_SIZE] = "";
    char *str_p;

    errorCode actual;
    errorCode expected;

    str_p = strncpy(str, "              fgdja            ", BUFFER_SIZE);
    expected = OK;

    clean_token(&str_p);
    actual = check_cleaned_token(str_p);

    assert(name, &actual, &expected, INT);

    str_p = strncpy(str, "        asf      fgdja            ", BUFFER_SIZE);
    expected = MISSING_COMMA;

    clean_token(&str_p);
    actual = check_cleaned_token(str_p);

    assert(name, &actual, &expected, INT);
}