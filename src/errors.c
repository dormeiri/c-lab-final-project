#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "errors.h"
#include "commons.h"

#define PRINT_ERR(STREAM, ERR) {\
    fprintf(STREAM, "%s:%d: error:\n\t%s\n\t%s\n", ERR->filename, ERR->line_num, error_code_msg(ERR->code), ERR->line_str);\
}

#define LOG_FILENAME(DEST) {\
    time_t datetime;\
    if(time(&datetime) == -1)\
    {\
        *(DEST) = "errors.log";\
    }\
    else\
    {\
        strftime(*(DEST), sizeof(*(DEST)) * 8, "errors_%Y%m%d%H%M.log", localtime(&datetime));\
    }\
}

void flush_error(error *error_ref);
static char *error_code_msg(errorCode code);



void flush_error(error *error_ref)
{
    FILE *error_log = NULL;

    PRINT_ERR(stderr, error_ref);
    if(!error_log)
    {
        char *filename;
        filename = (char *)malloc(sizeof(char) * MAX_STRING_LEN);
        LOG_FILENAME(&filename);
        if(!(error_log = fopen(filename, "a")))
        {
            exit(EXIT_FAILURE);
        }
        free(filename);
    }
    PRINT_ERR(error_log, error_ref);
    fclose(error_log);
    free(error_ref);
}

void create_error(errorCode code, int line_num, const char *filename, const char *line_str)
{
    error *err;
    if(!(err = (error *)malloc(sizeof(error))))
    {
        exit(EXIT_FAILURE);
    }
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
        "Invalid command line arguments, please insert one assembler code file", /* 15 */
        "16",
        "17",
        "18",
        "19",
        "20"
        /* TODO: Complete and make sure */
    };

    return error_msgs[code];
}