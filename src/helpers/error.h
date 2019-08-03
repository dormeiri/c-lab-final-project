/* 
*   Documenting:        DONE
*   Naming:             DONE
*   Code complexity:    DONE
*   Unused check:       DONE
*   Encapsulation:      DONE
*/

/*  
*   Errors handling guidelines:
*   - Any function that may catch any error during run-time should be with ErrorCode return type
*   - The actual output of the function will be as reference paramter with the name "out" 
*   - To throw wheter function with ErrorCode return type succeed use the macro TRY_THROW(FUNC)
*/

#ifndef ERRORS_HEADER
#define ERRORS_HEADER

/* 
*   Description:    Do FUNC, if any error occured, throw (return) the returned error from FUNC
*                   This is working only inside a function with ErrorCode return type
*   Parameters:
*   - FUNC: A function with ErrorCode return type. 
*           If FUNC returned is not OK, throw (return ErrorCode), otherwise do nothing.
*/
#define TRY_THROW(FUNC) {\
    ErrorCode res;\
    if((res = FUNC) != OK) \
        return res;\
}

/* Check wheter ErrorCode is ok indicator */
#define IS_OK(CODE)     ((CODE) < 0)

/* Check wheter ErrorCode is warning */
#define IS_WRN(CODE)    ((CODE) >= 100)

/* Check wheter ErrorCode is error */
#define IS_ERR(CODE)    (!IS_WRN(CODE) && !IS_OK(CODE))

/* The error code of a function, negative values are not error */
typedef enum ErrorCode
{
    /* If you declare new ErrorCode, add its message in error.c->error_msg function */
    /* TODO: Check if all error codes are actually used, check numbering */

    /** Ok indicators **/

    /* EOF found */
    EOF_OCCURED = -2,

    /* Function ran with no exceptions */
    OK = -1,   

    /** Errors **/

    /* Expected integer number, but parsed something else */
    NOT_WORD_NUM = 0,       

    /* Assign of reserved word */
    RESERVED_WORD = 1,

    /* Couldn't find the command */
    UNDEFINED_COMMAND = 2,    

    /* Extraneous text after end of command */
    AFTER_TEXT = 3,    

    /* Expected another paramater, but didn't found one on parsing */
    MISSING_PARAM = 4,    

    /* Encountered comma in exceptional place in the prompt */
    ILLEGAL_COMMA = 5,    

    /* Encountered white space instead of comma between arguments */
    MISSING_COMMA = 6,    

    /* Two or more consecutive commas has been found on parsing */
    CONSECUTIVE_COMMA = 7,   

    /* Invalid tag */
    INVALID_TAG = 8,

    /* Comma found and expecting another argument, but the value is empty */
    EMPTY_VAL = 10,   

    /* Expected arguments, but none found */
    ARGS_EXPECTED = 11,

    /* Exceeded buffer length on scanning */
    BUF_LEN_EXCEEDED = 12,

    /* Exceeded cleaned token length */
    TOK_LEN_EXCEEDED = 13,

    /* Error occured while reading/writing to a file */
    FILE_ERROR = 14,

    /* Invalid command line arguments */
    INVALID_CL = 15,

    /* Invalid address, for example r8 (not valid register) or a[[3] etc. */
    INVALID_ADDRESS = 16,

    /* Invalid string for .string statement. for exmaple: "fsa"fssss" */
    INVALID_STRING = 17,

    /*invalid combination of label in a macro statement */
    INVALID_COMB_LABEL_MACRO = 18,
    
    /* Macro statement structure is invalid */
    INVALID_MACRO_STATEMENT = 19,

    /* Symbol name declared twice */
    SYMBOL_ALREADY_EXIST = 20,

    /* Has more operands than expected */
    TOO_MANY_OPERANDS = 21,

    /* Symbol not declared */
    NOT_DECLARED = 22,

    /* Array has [ but not ] */
    MISSING_ARRAY_BRACE = 23,

    NOT_ARRAY_INDEX = 24,

    /* Entry already declared */
    ENTRY_ALREADY_EXISTS = 25,

    /* Token looks like register but the register is not defined in the machine */
    OUT_OF_RANGE_REGISTER = 26,

    /** Warnings **/

    /* Symbol declared but never used */
    UNUSED_SYMBOL = 100
} ErrorCode;

/*
*   Description:    Print error
*   Params:         (See error struct fields)
*/
void error_print(ErrorCode code, int line_num, const char *asm_name, const char *line_str, const char *info);


#endif