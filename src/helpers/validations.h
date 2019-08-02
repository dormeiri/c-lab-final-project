#ifndef VALIDATIONS_HEADER
#define VALIDATIONS_HEADER

#include "error.h"

/*  Description:    Check for bad chars at the beginning of arguments string
*   Params:
*       - args_str: The arguments string to check
*   Return:         If invalid chars found, return error code, otherwise, return OK
*/
ErrorCode preaction_validations(char *args_str);


/*  Description:    After a token is cleaned, validate that the token is valid
*   Parameters:
*       - token:    A cleaned token (see function: clean_token)
*   Return:         The validation result
*/
ErrorCode check_cleaned_token(char *token);

/*  Description:    Check if there is any DELIM_CHAR right after the token (ignore white spaces)
*   Parameters:
*       - token:    A token
*   Return:         CONSECUTIVE_COMMA if consecutive DELIM_CHAR found, otherwise OK
*/
ErrorCode check_token_consecutive(char *token);

/*  Description:    Check if the token is valid tag
*   Params:
*       - token:    The token to check
*   Return:         If not validate return the invlid reason with error code, otherwise OK
*/
ErrorCode is_valid_tag(char *token);

/*  Description:    Check for extraneous text after finish using arguments string (it use strtok(NULL,...) to do that)
*   Return:         If extraneous text found, return error code, otherwise OK
*/
ErrorCode postparsing_validations();

#endif