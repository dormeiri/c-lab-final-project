#include <stdio.h>
#include "errors.h"
#include "commons.h"

char *error_code_msg(errorCode code)
{
    static char *error_msgs[] = 
    {
        "Not int",
        "Reserved word",
        "Undefined command",
        "Extraneous text after end of statment",
        "Missing parameter",
        "Illegal comma",
        "Missing comma",
        "Consecutive commas",
        "Invalid character on declaring symbol",
        "Symbol name was not found",
        "Empty value",
        "Arguments expected",
        "Line exceeded max length",
        "Token exceeded max length"
    };

    return error_msgs[code];
}

void print_error(error error_obj)
{
    fprintf(stderr, "ERROR:\n\t%s (%s line %d)\n", error_code_msg(error_obj.code), error_obj.filename, error_obj.line_num);
    fprintf(stderr, "\tLine string: %s\n", error_obj.line_str);
    fprintf(stderr, "\tToken string: %s\n", error_obj.token_str);
}