#include "unit_test.h"
#include "../../src/errors.h"

void print_error_test();

int main()
{
    print_error_test();    

    return 0;
}

void print_error_test()
{
    error *err = create_error(NOT_WORD_NUM, 10, "test.c", "test_token", "line test_token, 532125, test");
    flush_error(err);
}