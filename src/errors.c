#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "errors.h"
#include "commons.h"

#define PRINT_ERR(STREAM, ERR) {\
    fprintf(STREAM, "%s:%d: error: %s (%d)\n\tline: %s\n", ERR->filename, ERR->line_num, error_code_msg(ERR->code), ERR->code, ERR->line_str);\
}

#define LOG_FILENAME(DEST) {\
    time_t DATETIME;\
    char *PTR = *(DEST);\
    if(time(&DATETIME) == -1)\
    {\
        PTR = "errors.log";\
    }\
    else\
    {\
        strftime(PTR, sizeof(*(DEST)) * 8, "errors_%Y%m%d%H%M%S.log", localtime(&DATETIME));\
    }\
}

void flush_error(error *error_ref);
static char *error_code_msg(errorCode code);

void create_error(errorCode code, int line_num, const char *asm_name, const char *line_str)
{
    error *err;
    if(!(err = (error *)malloc(sizeof(error))))
    {
        exit(EXIT_FAILURE);
    }
    err->code = code;
    err->line_num = line_num;
    err->filename = asm_name;
    err->line_str = line_str;
    flush_error(err);
}

void flush_error(error *error_ref)
{
    static char error_log_filename[MAX_STRING_LEN] = {'\0'};
    FILE *error_log;

    PRINT_ERR(stderr, error_ref);
    if(error_log_filename[0] == '\0')
    {
        LOG_FILENAME(&error_log_filename);
    }
    if(!(error_log = fopen(error_log_filename, "a")))
    {
        exit(EXIT_FAILURE);
    }
    PRINT_ERR(error_log, error_ref);
    fclose(error_log);
    free(error_ref);
}

char *error_code_msg(errorCode code)
{
    /*TODO: TBD, maybe change to simple switch case? */
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
        "Invalid address",
        "Invalid string",
        "Macro statement cannot be labaled",
        "Invalid macro statement",
        "Symbol already exists",
        "Too many operands",
        "Not declared",
        "Missing array brace",
        "Invalid array index",
        "Entry already declared"
        /* TODO: Complete and make sure */
    };

    return error_msgs[code];
}