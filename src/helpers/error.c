/* 
*   Documenting:        DONE
*   Naming:             DONE
*   Code complexity:    DONE
*   Unused check:       DONE
*   Encapsulation:      DONE
*   Includes:           DONE
*   Order:              DONE
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "error.h"
#include "../commons.h"

/* Represents error occurence while compiling .as file */
typedef struct
{
    ErrorCode   code;       /* Error code of the error */
    int         line_num;   /* Line number where the error occured */
    const char  *asm_name;  /* The name of file where the error occured */
    const char  *line_str;  /* The line string which caused the error */
    const char  *info;      /* Additional information about the error */
} Error;

static void error_log_filename(char *dest, size_t dest_size);
static void error_write(Error *e, FILE *first_stream, ...);
static const char *error_msg(ErrorCode code);

/* Creates new error struct and prints it to errors files streams. */
void error_print(ErrorCode code, int line_num, const char *asm_name, const char *line_str, const char *info)
{
    static char log_filename[MAX_STRING_LEN] = {'\0'}; /* Store the error log file name */
    FILE *error_log; /* Error log file stream pointer */
    Error e; /* Error struct that contains error information */

    e.code = code;
    e.line_num = line_num;
    e.asm_name = asm_name;
    e.line_str = line_str;
    e.info = info;

    /* If log_filename does not exists, create new log filename */
    if(!(*log_filename))
    {
        error_log_filename(log_filename, MAX_STRING_LEN);
    }

    /* Open log file and print error to that file */

    if(!(error_log = fopen(log_filename, "a")))
    {
        exit(EXIT_FAILURE);
    }
    error_write(&e, stderr, error_log, NULL);

    fclose(error_log);
}


/* Generate error log file by the current date and time */
void error_log_filename(char *dest, size_t dest_size)
{
    time_t datetime;

    /* If could not get the current time use default error log filename */
    if(time(&datetime) == -1)
    {
        strcpy(dest, "errors.log");
    }
    else
    {
        strftime(dest, dest_size, "errors_%Y%m%d%H%M%S.log", localtime(&datetime));\
    }
}

/* Print error message to stream  based on the data exists in e */
void error_write(Error *e, FILE *first_stream, ...)
{
    const char *type;
    FILE *stream;
    va_list ap;
    va_start(ap, first_stream);

    /* Find if the ErrorCode is warning or error */
    if(IS_WRN(e->code))
    {
        type = "warning";
    }
    else 
    {
        type = "error";
    }

    /* Print to all streams found in unnamed argument */
    for(stream = first_stream; stream; stream = va_arg(ap, FILE *))
    {
        /* Print error message */
        fprintf(stream, "%s:%d: %s: %s (%d)", e->asm_name, e->line_num, type, error_msg(e->code), e->code);

        /* If any addition info exists, print it */
        if(e->info)
        {
            fprintf(stream, ", %s", e->info);
        }

        /* Print the line which created the error */
        fprintf(stream, "\n\tline: %s\n\n", e->line_str);
    }
    va_end(ap);
}

/* Get the error message string by ErrorCode */
const char *error_msg(ErrorCode code)
{
    switch (code)
    {
        case NOT_WORD_NUM: return "Expected integer number";
        case RESERVED_WORD: return "Reserved word";
        case UNDEFINED_COMMAND: return "Undefined command";
        case AFTER_TEXT: return "Extraneous text after end of statment";
        case MISSING_PARAM: return "Missing parameter";
        case ILLEGAL_COMMA: return "Illegal comma";
        case MISSING_COMMA: return "Missing comma";
        case CONSECUTIVE_COMMA: return "Consecutive commas";
        case INVALID_TAG: return "Invalid character on declaring symbol";
        case EMPTY_VAL: return "Empty value";
        case ARGS_EXPECTED: return "Arguments expected";
        case BUF_LEN_EXCEEDED: return "Line exceeded max length";
        case TOK_LEN_EXCEEDED: return "Token exceeded max length";
        case FILE_ERROR: return "Error while trying to open a file";
        case INVALID_CL: return "Invalid command line arguments";
        case INVALID_ADDRESS: return "Invalid address";
        case INVALID_STRING: return "Invalid string";
        case INVALID_COMB_LABEL_MACRO: return "Macro statement cannot be labaled";
        case INVALID_MACRO_STATEMENT: return "Invalid macro statement";
        case SYMBOL_ALREADY_EXIST: return "Symbol already declared";
        case TOO_MANY_OPERANDS: return "Too many operands";
        case NOT_DECLARED: return "Not declared";
        case MISSING_ARRAY_BRACE: return "Missing array brace";
        case NOT_ARRAY_INDEX: return "Invalid array index";
        case ENTRY_ALREADY_EXISTS: return "Entry already declared";
        case UNUSED_SYMBOL: return "Symbol declared but never used";
        case OUT_OF_RANGE_REGISTER: return "Unknown register";
        default: return NULL;
    }
}