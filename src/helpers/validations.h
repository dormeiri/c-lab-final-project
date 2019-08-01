#ifndef VALIDATIONS_HEADER
#define VALIDATIONS_HEADER

#include "error.h"

/*  Description:    Check if the token is valid tag
*   Params:
*       - token:    The token to check
*   Return:         If not validate return the invlid reason with error code, otherwise OK
*/
ErrorCode is_valid_tag(char *token);

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


#endif