#ifndef PARSING_HEADER
#define PARSING_HEADER

#include "../commons.h"
#include "../errors.h"

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

/* 
Description:    Split command line to command name and arguments string,
                This method will also change command_line
Parameters:
    - command_line: String of the full command line.
    - command_strp: A pointer to the string of the command name part in the command line.
    - args_strp:    A pointer to the string of the arguments part in the command line.
*/
void split_command_line(char *command_line, char **command_strp, char **args_strp);

/* 
Description:    Parse the next token with strtok, and then parse the token to a real number and store it in nump
Parameters:
    - args_str:     For the first token, put the string of the arguments part in the command line,
                    after the first token, put NULL and the function will read the next token.
    - indexp:       A pointer to a real number
Return:         OK if ran as expected, otherwise, error code
*/
errorCode strtok_num(char *args_str, double *nump);

/*
Description:    Cleans a token from trailing and leading white spaces
Parameters:
    - tokenp:    A pointer to the string you want to clean
*/
void clean_token(char **tokenp);

#endif