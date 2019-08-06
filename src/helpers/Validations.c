#include "Validations.h"
#include "Parsing.h"
#include "../Commons.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Check if there is DELIM_CHAR in command_str, 
then check if the first character in args_str is DELIM_CHAR */
ErrorCode preaction_validations(char *args_str)
{
    if(!IS_EMPTY_STR(args_str) && *args_str == DELIM_CHAR)
    {
        return ILLEGAL_COMMA;
    }
    return OK;
}

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

ErrorCode is_valid_tag(const char *token)
{
    Word temp;
    char i = 1;

    /* TODO: Add reserved words validations  */
    /*TODO add an assumption is assembler.c that r165 is an invalid label name  */
    /*TODO add the assumtions we made throughout the entire file */
    /*TODO go over the booklet and add ALL assumptions, even the ones they assumed (unluss super obvious) */

    if((parse_register(token, &temp)) || parse_operation_type(token) != NONE_OP)
    {
        return INVALID_TAG;
    }

    TRY_THROW(isalpha(*token));
    for(token++; i >= MAX_TAG_LEN || isalnum(*token); token++, i++);
    
    return *token ? INVALID_TAG : OK;
}

/* Check if strtok is not returning NULL, then it means that there is unexpected DELIM_CHAR in the command line */
ErrorCode postparsing_validations()
{
    if(strtok(NULL, DELIM) != NULL)
    {
        return AFTER_TEXT;
    }

    return OK;
}