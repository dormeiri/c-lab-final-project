#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "validations.h"
#include "parsing.h"

/* TODO: Move to validations? */
#define INSTANT_CHAR '#'
#define INDEX_START '['
#define INDEX_END ']'
#define TAG_END ':'
#define REGISTER_CHAR 'r'
#define REGISTER_INDEX(STR) (STR[1] - '0')
#define IS_NUM_FIRST_CHAR(STR) (isdigit(STR[0]) || STR[0] == '-' || STR[0] == '+')
#define IS_REGISTER(STR) (STR[0] == REGISTER_CHAR && STR[1] != '\0' && STR[2] == '\0')
#define IS_VALID_REGISTER(STR) (IS_REGISTER(STR) && REGISTER_INDEX(STR) >= 0 && REGISTER_INDEX(STR) < NUM_OF_REGISTERS && STR[2] == '\0')

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

errorCode tok_to_num(char *token, word *num_ref);
errorCode tok_index_to_num(char *token, word *num_ref);
errorCode strtok_wrapper(char *args_str, char **tokenp);
errorCode map_statement_key(char *statement_key_str, statement *statement_ref);
errorCode map_operation_type(char *statement_key_str, statement *statement_ref);

errorCode parse_args(int argc, char *argv[], char **input_filename)
{
    if(argc != 2)
    {
        return INVALID_CL;
    }

    *input_filename = *(argv + 1);
    
    return OK;
}

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
void clean_token(char **token_ref)
{
    str_len_t i;

    /* Clean the leading white spaces */
    IGNORE_WHITE_SPACES(*token_ref);

    /* Check if every character in the token was white space, if not, clean trailing white spaces */
    if(IS_EMPTY_STR(*token_ref) == FALSE)
    {
        for(i = strlen(*token_ref) - 1; i >= 0 && IS_WHITESPACE((*token_ref)[i]); i--);
        (*token_ref)[i + 1] = '\0';
    }
}

errorCode get_next_arg(char *args_str, address *address_ref)
{
    errorCode res;
    char *token;
    /* TODO: symbol temp_symbol; */

    TRY_THROW(res,strtok_wrapper(args_str, &token));

    if(IS_NUM_FIRST_CHAR(token))
    {
        TRY_THROW(res, tok_to_num(token, &address_ref->value));
        address_ref->type = DATA;
    }
    else if(token[0] == INSTANT_CHAR)
    {
        token++;
        TRY_THROW(res, tok_to_num(token, &address_ref->value));
        address_ref->type = INSTANT;
    }
    else if(IS_VALID_REGISTER(token))
    {
        address_ref->value = REGISTER_INDEX(token);
        address_ref->type = REGISTER;
    }
    else if(is_valid_tag(token))
    {
        /* TRY_THROW(res, get_symbol(token, &temp_symbol)); */
        /* address_ref->value + temp_symbol->value; */
    }
    else
    {
        TRY_THROW(res, tok_index_to_num(token, &address_ref->value));
        address_ref->type = INDEX;
    }
    

    return OK;
}


/***** Privates *****/

errorCode map_statement_key(char *statement_key_str, statement *statement_ref)
{
    static translator translator_arr[] = 
    {
        {".data",   DATA_KEY},
        {".string", STRING_KEY},
        {".define", MACRO},
        {".entry",  ENTRY},
        {".extern", EXTERN},
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

/* Parse the next token with strtok, clean this token from leading and trailing white spaces,
and check if the token is valid pass args_str as NULL to continue reading tokens from the last string */
errorCode strtok_wrapper(char * args_str, char **token_ref)
{
    errorCode res;    /* The result of the function */
    str_len_t i;

    if((*token_ref = strtok(args_str,DELIM)) == NULL)
    {
        return MISSING_PARAM;
    }

    TRY_THROW(res, check_token_consecutive(*token_ref));

    clean_token(token_ref);
    /* Not using strlen because we need only the first 16 chars */
    for(i = 0; (*token_ref)[i] != '\0'; i++)
    {
        if(i == MAX_TOKEN_LEN)
        {
            return TOK_LEN_EXCEEDED;
        }
    }

    if(IS_EMPTY_STR(*token_ref))
    {
        /* There is no delimiter after this token, and this token is empty */
        return args_str == NULL ? EMPTY_VAL : ARGS_EXPECTED;
    }

    TRY_THROW(res, check_cleaned_token(*token_ref));

    return OK;
}

errorCode tok_to_num(char *token, word *num_ref)
{
    char *end_str;          /* The pointer to the string after the parsed number */
    int num;

    num = strtol(token, &end_str, 10);

    /* If there is any character after the read number, that mean that the token didn't containd just a number */
    if(abs(*num_ref) > MAX_NUM || IS_EMPTY_STR(end_str) == FALSE)
    {
        return NOT_WORD_NUM;
    }

    *num_ref = num;
    return  OK;
}

errorCode tok_index_to_num(char *token, word *num_ref)
{
    errorCode res;
    char *temp_tok = token;
    /* TODO: symbol temp_symbol; */
    
    /* Skip to first occurence of INDEX_START and validate */
    for(; (*temp_tok != INDEX_START) && (IS_EMPTY_STR(temp_tok) == FALSE); temp_tok++);
    TRY_THROW(res, IS_EMPTY_STR(temp_tok) ? INVALID_ADDRESS : OK);
    SPLIT_STR(temp_tok);
    /* TODO: TRY_THROW(res, get_symbol(token, &temp_symbol)); */

    /* Skip to first occurence of INDEX_END and validate */
    token = temp_tok;
    for(; (*temp_tok != INDEX_END) && (IS_EMPTY_STR(temp_tok) == FALSE); temp_tok++);
    TRY_THROW(res, IS_EMPTY_STR(temp_tok) ? INVALID_ADDRESS : OK);
    SPLIT_STR(temp_tok);

    /* Validate this is the end of the token */
    TRY_THROW(res, IS_EMPTY_STR(temp_tok) ? OK : INVALID_ADDRESS);

    /* Set number */
    TRY_THROW(res, tok_to_num(token, num_ref));
    if(*num_ref < 0)
    {
        return INVALID_ADDRESS;
    }
    /* num_ref->value += temp_symbol->value; */
    return OK;
}