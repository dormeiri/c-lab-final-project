#include "validations.h"
#include "parsing.h"
#include "../commons.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

ErrorCode is_valid_tag(char *token)
{
    char i = 1;

    /* TODO: Add reserved words validations */
    TRY_THROW(isalpha(*token));
    for(token++; i > MAX_TAG_LEN || isalnum(*token); token++, i++);
    return *token ? INVALID_TAG : OK;
}


/* Check if there is DELIM_CHAR in command_str, 
then check if the first character in args_str is DELIM_CHAR */
/*ErrorCode preaction_validations(char *command_str, char *args_str)
{
    for(; *command_str != '\0'; command_str++)
    {
        if(*command_str == DELIM_CHAR)
        {
            return ILLEGAL_COMMA;
        }
    }

    if(*args_str == DELIM_CHAR)
    {
        return ILLEGAL_COMMA;
    }
    return OK;
}*/ /* Decide if we want to delete */

/* Check if strtok is not returning NULL, then it means that there is unexpected DELIM_CHAR in the command line */
/*ErrorCode postparsing_validations()
{
    if(strtok(NULL, DELIM) != NULL)
    {
        return AFTER_TEXT;
    }

    return OK;
}*/ /* TODO: Decide if we want to delete */

/* Check for white spaces in a cleaned token */
ErrorCode check_cleaned_token(char *token)
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
ErrorCode check_token_consecutive(char *token)
{
    token += strlen(token) + 1;
    IGNORE_WHITE_SPACES(token);
    if(*token == DELIM_CHAR)
    {
        return CONSECUTIVE_COMMA;
    }

    return OK;
}