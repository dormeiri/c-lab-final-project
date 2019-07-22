#include <stdlib.h>
#include <stdio.h>
#include "errors.h"
#include "commons.h"

void flush_error(error *error_ref);
static char *error_code_msg(errorCode code);

#define PRINT_ERR(STREAM, ERR) {\
    fprintf(STREAM, "ERROR:\t%s (%s, line %d)\n", error_code_msg(ERR->code), ERR->filename, ERR->line_num);\
    fprintf(STREAM, "\tLine string: %s\n", ERR->line_str);\
}

void flush_error(error *error_ref)
{
    FILE *error_log;

    PRINT_ERR(stderr, error_ref);
    if(!error_log)
    {
        error_log = fopen(error_ref->filename, "a");
    }
    PRINT_ERR(error_log, error_ref);
    fclose(error_log);
    free(error_ref);
}

void create_error(errorCode code, int line_num, const char *filename, const char *line_str)
{
    error *err = (error *)malloc(sizeof(error));
    err->code = code;
    err->line_num = line_num;
    err->filename = filename;
    err->line_str = line_str;
    flush_error(err);
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
        /* TODO: Complete and make sure */
    };

    return error_msgs[code];
}