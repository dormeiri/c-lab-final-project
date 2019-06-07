#include <string.h>
#include <stdlib.h>

#include "validations.h"
#include "parsing.h"

#define TAG_END ':'
errorCode strtok_wrapper(char *args_str, char **tokenp);

/* Ignore leading white spaces, replace the last character if it is a newline
then search the first white space occures and split there to command name and args_strp */
errorCode split_statement(char *statement_line, char **tag_ref, char **statement_key_ref, char **args_ref)
{   
    errorCode res;

    IGNORE_WHITE_SPACES(statement_line);
    *tag_ref = statement_line;

    for(; (IS_WHITESPACE(*statement_line) || *statement_line == TAG_END) == FALSE; statement_line++);
    if(*statement_line == TAG_END)
    {
        /* Tag found */
        *statement_line = '\0'; /* Split */
        
        /* TODO: Validate tag */
        /* TRY_THROW(res, tag_validation(*tag)); */

        statement_line++;

        IGNORE_WHITE_SPACES(statement_line);
        *statement_key_ref = statement_line;

        for(; IS_WHITESPACE(*statement_line) == FALSE; statement_line++);
        *statement_line = '\0'; /* Split */
    }
    else
    {
        /* Tag not found */
        *statement_line = '\0'; /* Split */

        *statement_key_ref = *tag_ref;
        *tag_ref = NULL;
    }
    
    statement_line++;
    *args_ref = statement_line;

    return OK;
}

/* Use strtok_wrapper to get a clean token, then use strtod to parse the token into number,
that number will be stored in nump, if the token wasn't valid return error code */
errorCode strtok_num(char *args_str, double *nump)
{
    char *end_str;          /* The pointer to the string after the parsed number */
    char *token;            /* Cleaned token from strtok_wrapper */
    errorCode res;          /* The result of the function */

    TRY_THROW(res, strtok_wrapper(args_str, &token));

    *nump = strtol(token, &end_str, 10);

    /* If there is any character after the read number, that mean that the token didn't containd just a number */
    return IS_EMPTY_STR(end_str) ? OK : NOT_INT;
}

/* Advence the pointer to the token by one for each white space found,
and put '\0' in the last white space found from the end */
void clean_token(char **tokenp)
{
    str_len_t i;

    /* Clean the leading white spaces */
    IGNORE_WHITE_SPACES(*tokenp);

    /* Check if every character in the token was white space, if not, clean trailing white spaces */
    if(IS_EMPTY_STR(*tokenp) == FALSE)
    {
        for(i = strlen(*tokenp) - 1; i >= 0 && IS_WHITESPACE((*tokenp)[i]); i--);
        (*tokenp)[i + 1] = '\0';
    }
}

/* Parse the next token with strtok, clean this token from leading and trailing white spaces,
and check if the token is valid pass args_str as NULL to continue reading tokens from the last string */
errorCode strtok_wrapper(char * args_str, char **tokenp)
{
    errorCode res;    /* The result of the function */
    str_len_t i;

    if((*tokenp = strtok(args_str,DELIM)) == NULL)
    {
        return MISSING_PARAM;
    }

    TRY_THROW(res, check_token_consecutive(*tokenp));

    clean_token(tokenp);
    /* Not using strlen because we need only the first 16 chars */
    for(i = 0; (*tokenp)[i] != '\0'; i++)
    {
        if(i == MAX_TOKEN_LEN)
        {
            return TOK_LEN_EXCEEDED;
        }
    }

    if(IS_EMPTY_STR(*tokenp))
    {
        /* There is no delimiter after this token, and this token is empty */
        return args_str == NULL ? EMPTY_VAL : ARGS_EXPECTED;
    }

    TRY_THROW(res, check_cleaned_token(*tokenp));

    return OK;
}