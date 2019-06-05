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
    error err = {
        NOT_INT,
        10,
        "test.c",
        "test_token",
        "line test_token, 53125, test"
    };

    print_error(err);
}