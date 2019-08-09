#include "Validations.h"
#include "Parsing.h"
#include "../Commons.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Check if there is DELIM_CHAR in command_str, 
then check if the first character in args_str is DELIM_CHAR */
ErrorCode preaction_validations(const char *args_str)
{
    if(!IS_EMPTY_STR(args_str))
    {
        if(args_str[strlen(args_str) - 1] == DELIM_CHAR)
        {
            return AFTER_TEXT;
        }
        if(*args_str == DELIM_CHAR)
        {
            return ILLEGAL_COMMA;
        }
    }
    return OK;
}

/* Check for white spaces in a cleaned token */
ErrorCode check_cleaned_token(const char *token)
{
    for(; !IS_EMPTY_STR(token); token++)
    {
        if(IS_WHITESPACE(*token))
        {
            return MISSING_COMMA;
        }
    }

    return OK;
}

/* Check if the first letter after token string is DELIM_CHAR (ignoring white spaces) */
ErrorCode check_token_consecutive(const char *token)
{
    token += strlen(token) + 1;
    IGNORE_WHITE_SPACES(token);
    if(*token == DELIM_CHAR)
    {
        return CONSECUTIVE_COMMA;
    }

    return OK;
}

ErrorCode is_valid_tag(const char *token)
{
    Word temp;
    char i = 1;

    if((parse_register(token, &temp)) || parse_operation_type(token) != NONE_OP)
    {
        return INVALID_TAG;
    }

    TRY_THROW(isalpha(*token) ? OK : INVALID_TAG);

    for(token++; i >= MAX_TAG_LEN || isalnum(*token); token++, i++);
    return *token ? INVALID_TAG : OK;
}