/* 
*   Documenting:        DONE
*   Naming:             DONE
*   Code complexity:    DONE
*   Unused check:       DONE
*   Encapsulation:      DONE
*   Order:              DONE
*/

#ifndef PARSING_HEADER
#define PARSING_HEADER

#include "../step_one.h"

/* Check whether a character is white space character */
#define IS_WHITESPACE(CHAR) ((CHAR) == WHITE_SPACE_1 || (CHAR) == WHITE_SPACE_2)

/* Check whether a string is empty */
#define IS_EMPTY_STR(STR)   (!(STR) || !(*(STR)))

/* 
* Go to the first non whitespace char 
* STR: the string to manipulate
*/
#define IGNORE_WHITE_SPACES(STR) for(;IS_WHITESPACE(*(STR)); ((STR))++)


/*  Description:    Get thecurrent line step one is working on, 
*                   parse and map it and put the result as the current statement struct of step_one
*   Params:
*       - step_one: The step one struct that contains current line
*   Return:         If any error occured, return error type, otherwise OK
*/
ErrorCode map_statement(step_one *step_one);

/*  Description:    Parse the arguments of macro statement, puts in symbol and value parameters the result
*   Params:
*       - step_one: The step one struct that contains current statement with macro type
*       - symbol:   Set here the symbol name as declared in the macro arguments
*       - value:    Set here the value as declared in the macro arguments
*   Return:         If any error occured, return error type, otherwise OK
*/
ErrorCode parse_macro_statement(step_one *step_one, char **symbol, word *value);

/*  Description:    Parse the next argument of step_one current statement struct and put it in out
*   Params:
*       - step_one: The step one struct that contains current statement struct
*       - out:      The result will go here
*   Return:         If any error occured, return error type, otherwise OK
*/
ErrorCode get_next_arg(step_one *step_one, address **out);

/*  Description:    Clean and parse the arguments string as a .string argument
*   Params:
*       - step_one: The step one struct that contains current statement struct
*       - out:      The result will go here
*   Return:         If any error occured, return error type, otherwise OK
*/
ErrorCode get_string_arg(step_one *step_one, char **out);

/*  Description:    Get the argument string as a label for .entry and .extern
*   Params:
*       - step_one: The step one struct that contains current statement struct
*       - out:      The result will go here
*   Return:         If any error occured, return error type, otherwise OK
*/
ErrorCode get_label_arg(step_one *step_one, char **out);

#endif