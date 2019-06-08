#include <string.h>
#include <stdlib.h>

#include "validations.h"
#include "parsing.h"

#define TAG_END ':'

/* Split string in its current pointer position */
#define SPLIT_STR(STR) *(STR++) = '\0'

/* Read statment first word into DEST */
#define READ_FIRST_WORD(STR, DEST) \
    IGNORE_WHITE_SPACES(STR);\
    DEST = STR;\
    for(;(IS_EMPTY_STR(STR) || IS_WHITESPACE(*STR) || *STR == TAG_END) == FALSE; STR++)

/* Read statement second word into DEST */
#define READ_SECOND_WORD(STR, DEST) \
    IGNORE_WHITE_SPACES(STR);\
    DEST = STR;\
    for(;(IS_EMPTY_STR(STR) || IS_WHITESPACE(*STR)) == FALSE; STR++)

errorCode strtok_wrapper(char *args_str, char **tokenp);
errorCode map_statement_key(char *statement_key_str, statement *statement_ref);
errorCode map_operation_type(char *statement_key_str, statement *statement_ref);

errorCode map_statement(char *statement_line, statement *statement_ref)
{   
    errorCode res;
    char *statement_key;

    READ_FIRST_WORD(statement_line, statement_ref->tag);
    if(*statement_line == TAG_END)
    {
        SPLIT_STR(statement_line);
        READ_SECOND_WORD(statement_line, statement_key);
    }
    else
    {
        statement_key = statement_ref->tag;
        statement_ref->tag = NULL;
    }
    
    if(IS_EMPTY_STR(statement_line) == FALSE)
    {
        SPLIT_STR(statement_line);
        IGNORE_WHITE_SPACES(statement_line);
    }
    statement_ref->args = IS_EMPTY_STR(statement_line) ? NULL : statement_line;

    TRY_THROW(res, map_statement_key(statement_key, statement_ref));
    return OK;
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

errorCode map_statement_key(char *statement_key_str, statement *statement_ref)
{
    static translator translator_arr[] = 
    {
        {".data", DATA_KEY},
        {".string", STRING_KEY},
        {".define", MACRO},
        {NULL, 0}
    };
    errorCode res;
    unsigned char i;

    if(statement_key_str[0] == '.')
    {
        for(i = 0; strcmp(translator_arr[i].str, statement_key_str); i++);
        if(translator_arr[i].str == NULL)
        {
            return UNDEFINED_COMMAND; /* TODO: Undefined statement key? undefined operation? */
        }
        statement_ref->statement_type = translator_arr[i].type;
        statement_ref->operation_type = NONE;
    }
    else
    {
        statement_ref->statement_type = OPERATION;
        TRY_THROW(res, map_operation_type(statement_key_str, statement_ref));
    }

    return OK;
}

errorCode map_operation_type(char *statement_key_str, statement *statement_ref)
{
    static translator translator_arr[] = 
    {
        {"mov", MOV},
        {"jmp", JMP},
        {"add", ADD},
        {"sub", SUB},
        {NULL, 0}
    };
    unsigned char i;

    for(i = 0; strcmp(translator_arr[i].str, statement_key_str); i++);
    if(translator_arr[i].str == NULL)
    {
        return UNDEFINED_COMMAND; /* TODO: Undefined statement key? undefined operation? */
    }
    statement_ref->operation_type = translator_arr[i].type;

    return OK;
}

errorCode parse_args(int argc, char *argv[], char **input_filename, char **output_filename)
{
    if(argc != 3)
    {
        return INVALID_CL;
    }

    *input_filename = *(argv + 1);
    *output_filename= *(argv + 2);
    
    return OK;
}