#ifndef VALIDATIONS_HEADER
#define VALIDATIONS_HEADER

#include "error.h"

/* 
Description:    Does first validation of the command line string
Parameters:
    - command_str:  String of the command name part in the command line,
    - args_str:     String of the arguments part in the command line,
Return:         The validation result
*/
ErrorCode preaction_validations(char *command_str, char *args_str);

/* 
Description:    Does first validation of the command line string
Parameters:
    - command_str:  String of the command name part in the command line,
    - args_str:     String of the arguments part in the command line,
Return:         The validation result
*/
ErrorCode postparsing_validations();

/* 
Description:    After a token is cleaned, validate that the token is valid
Parameters:
    - token:    A cleaned token (see function: clean_token)
Return:         The validation result
*/
ErrorCode check_cleaned_token(char *token);

/* 
Description:    Check if there is any DELIM_CHAR right after the token (ignore white spaces)
Parameters:
    - token:    A token
Return:         CONSECUTIVE_COMMA if consecutive DELIM_CHAR found, otherwise OK
*/
ErrorCode check_token_consecutive(char *token);

/* 
Description:    Check wheter the args_str is empty
Parameters:
    - args_str:     String of the arguments part in the command line,
Return:         TRUE if args_str is empty, otherwise FALSE
*/
ErrorCode check_empty_args(char *args_str);

ErrorCode is_valid_tag(char *token);

#endif