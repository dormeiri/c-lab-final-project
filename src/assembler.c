#include <stdlib.h>
#include <stdio.h>
#include "assembler.h"
#include "helpers/parsing.h"

int main(int argc, char *argv[])
{
    error *err;
    errorCode res;
    char *input_filename;

    if((res = parse_args(argc, argv, &input_filename)) != OK)
    {
        err = create_error(res, -1, "", "", "");
        flush_error(err);
    }
    else
    {
        printf("%s\n", input_filename);
    }

    return 0;
}