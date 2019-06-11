#include <stdlib.h>
#include <stdio.h>
#include "errors.h"
#include "commons.h"

/* TODO: Do we need both create and flush? Can we do only one function that just print the error? */

static char *error_code_msg(errorCode code);

void flush_error(error *error_ref)
{
    fprintf(stderr, "ERROR:\t%s (%s, line %d)\n", error_code_msg(error_ref->code), error_ref->filename, error_ref->line_num);
    fprintf(stderr, "\tLine string: %s\n", error_ref->line_str);
    fprintf(stderr, "\tToken string: %s\n", error_ref->token_str);
    free(error_ref);
}

error *create_error(errorCode code, int line_num, char *filename, char *token_str, char *line_str)
{
    error *err = (error *)malloc(sizeof(error));
    err->code = code;
    err->line_num = line_num;
    err->filename = filename;
    err->token_str = token_str;
    err->line_str = line_str;

    return err;
}

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
        "Token exceeded max length",
        "Error while trying to open a file",
        "Invalid command line arguments, please insert one assembler code file"
    };

    return error_msgs[code];
}