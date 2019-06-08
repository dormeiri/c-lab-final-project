#include "validations.h"
#include "parsing.h"
#include "../commons.h"

/* Check if there is DELIM_CHAR in command_str, 
then check if the first character in args_str is DELIM_CHAR */
errorCode preaction_validations(char *command_str, char *args_str)
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
}

/* Check if strtok is not returning NULL, then it means that there is unexpected DELIM_CHAR in the command line */
errorCode postparsing_validations()
{
    if(strtok(NULL, DELIM) != NULL)
    {
        return AFTER_TEXT;
    }

    return OK;
}

/* Check for white spaces in a cleaned token */
errorCode check_cleaned_token(char *token)
{
    for(; IS_EMPTY_STR(token) == FALSE; token++)
    {
        if(IS_WHITESPACE(*token))
        {
            return MISSING_COMMA;
        }
    }

    return OK;
}

/* Check if the first letter after token string is DELIM_CHAR (ignoring white spaces) */
errorCode check_token_consecutive(char *token)
{
    token += strlen(token) + 1;
    IGNORE_WHITE_SPACES(token);
    if(*token == DELIM_CHAR)
    {
        return CONSECUTIVE_COMMA;
    }

    return OK;
}

errorCode check_empty_args(char *args_str)
{
    IGNORE_WHITE_SPACES(args_str);
    return IS_EMPTY_STR(args_str);
}