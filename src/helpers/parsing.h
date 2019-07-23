#ifndef PARSING_HEADER
#define PARSING_HEADER

#include "../commons.h"
#include "../errors.h"
#include "../step_one.h"

/* Check wheter a character is white space character */
#define IS_WHITESPACE(CHAR) (CHAR == WHITE_SPACE_1 || CHAR == WHITE_SPACE_2)

/* Check wheter a string is empty */
#define IS_EMPTY_STR(STR)   (*STR == '\0')

/* 
Iterating through characters until encountered non white space character, preserving the last state of I and CHAR
I:      indexer that increasing each white space found
CHAR:   The char to look at after each iteration
*/
#define IGNORE_WHITE_SPACES(STR) for(;IS_WHITESPACE(*STR); (STR)++)

typedef struct translator
{
    char *str;
    int type;
    
} translator;

/* Parse the arguments of macro statement */
errorCode parse_macro_statement(step_one *step_one, char **symbol, word *value);

/* 
Description:            Split command line to command name and arguments string,
                        This method will also change command_line
Parameters:
    - command_line:     String of the full command line.
    - statement_ref:    Reference to statement structs, this is the output
Return:                 OK if ran as expected, otherwise, error code
*/
errorCode map_statement(step_one *step_one);

/*
Description:    Cleans a token from trailing and leading white spaces
Parameters:
    - token_ref:    A pointer to the string you want to clean
*/
void clean_token(char **token_ref);

/* get the next argument of the line, address out is initialized inside the function */
errorCode get_next_arg(step_one *step_one, address **out);
errorCode get_string_arg(step_one *step_one, char **str_ref);

/* not yet implemented, used to get the label in the argument of entry and extern instructions*/
errorCode get_label_arg(step_one *step_one, char **label);

#endif