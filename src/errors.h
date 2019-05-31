#ifndef ERRORS_HEADER
#define ERRORS_HEADER

/* 
Description:    Do FUNC, if any error occured throw the returned error from FUNC by doing return
                This is working only inside a function that return errorCode
Parameters:     
    - RES:      Declared errorCode variable to store the result of FUNC.
    - FUNC:     A function that return errorCode, if the result of the function do return with the error returned from the function
*/
#define TRY_THROW(RES, FUNC) \
    if((RES = FUNC) != OK) \
        return RES

/* Is errorCode variable is an error */
#define IS_ERR(R) (R >= 0)

/* The error code of a function, negative value is not an error */
typedef enum errorCode
{
    /* Function ran with no exceptions */
    OK = -1,   

    /* Expected real number, but parsed something else */
    NOT_REAL = 0,    

    /* Expected valid mat name but couldn't found that parsed name */
    UNDIFINED_MAT = 1,    

    /* Couldn't find the command */
    UNDIFINED_COMMAND = 2,    

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

    /* Expected real number as a scalar, but parsed something else */
    NOT_SCALAR = 8,    

    /* Encountered EOF */
    EOF_ERROR = 9,    

    /* Comma found and expecting another argument, but the value is empty */
    EMPTY_VAL = 10,   

    /* Expected arguments, but none found */
    ARGS_EXPECTED = 11,

    /* Exceeded buffer length on scanning */
    BUF_LEN_EXCEEDED = 12,

    /* Exceeded cleaned token length */
    TOK_LEN_EXCEEDED = 13
} errorCode;

void print_error(errorCode error);

#endif