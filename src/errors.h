#ifndef ERRORS_HEADER
#define ERRORS_HEADER

/* 
Description:    Do FUNC, if any error occured throw the returned error from FUNC by doing return
                This is working only inside a function that return errorCode
Parameters:     
    - FUNC:     A function that return errorCode, if the result of the function do return with the error returned from the function
*/
#define TRY_THROW(FUNC) {\
    errorCode res;\
    if((res = FUNC) != OK) \
        return res;\
}

/* Is errorCode variable is an error */
#define IS_ERR(R) (R >= 0)

/* The error code of a function, negative value is not an error */
typedef enum errorCode
{
    /* EOF found */
    EOF_OCCURED = -2,

    /* Function ran with no exceptions */
    OK = -1,   

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
    
    /* Expected symbol, but did not found the symbol */
    UNDEFINED_SYMBOL = 9,

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
    INVALID_COMB_LABEL_MACRO = 18
} errorCode;

typedef struct error
{
    errorCode   code;       /* Error code of the error */
    int         line_num;   /* Line number where the error occured */
    char        *filename;  /* The name of file where the error occured */
    char        *line_str;  /* The line string which caused the error */
} error;

void create_error(errorCode code, int line_num, char *filename, char *line_str);

#endif