/* 
Guidelines: 
    - Invalid tag are checked in parsing level
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../symbols.h"
#include "validations.h"
#include "parsing.h"

#define MACRO_SET_CHAR '='
#define QUOTE_CHAR '"'
#define INSTANT_CHAR '#'
#define INDEX_START '['
#define INDEX_END ']'
#define TAG_END ':'
#define REGISTER_CHAR 'r'
#define REGISTER_INDEX(STR) (STR[1] - '0')
#define IS_NUM_FIRST_CHAR(STR) (isdigit(STR[0]) || STR[0] == '-' || STR[0] == '+')
#define IS_REGISTER(STR) (STR[0] == REGISTER_CHAR && STR[1] != '\0' && STR[2] == '\0')
#define IS_VALID_REGISTER(STR) (IS_REGISTER(STR) && REGISTER_INDEX(STR) >= 0 && REGISTER_INDEX(STR) < NUM_OF_REGISTERS && STR[2] == '\0')
#define IS_ARRAY(STR) (strchr(STR, '['))

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

static errorCode tok_to_num(step_one *step_one, char *token, word *num_ref);
static errorCode tok_to_array(step_one *step_one, char *token, address *address_ref);
static errorCode strtok_wrapper(step_one *step_one, char **tokenp);
static errorCode map_statement_key(char *statement_key_str, statement *statement_ref);
static errorCode map_operation_type(char *statement_key_str, statement *statement_ref);

errorCode get_label_arg(step_one *step_one, char **label)
{
    label = &step_one->curr_statement->args;
    clean_token(label);
    return is_valid_tag(*label);
}

errorCode parse_macro_statement(step_one *step_one, char **symbol, word *value)
{
    /* TODO: Break down to smaller funcitons, try to remove duplicates */

    char *args_str = step_one->curr_statement->args;
    IGNORE_WHITE_SPACES(args_str);
    symbol = &args_str;
    for(;!(IS_EMPTY_STR(args_str) || IS_WHITESPACE(*args_str) || *args_str == MACRO_SET_CHAR); args_str++);
    if(*args_str == MACRO_SET_CHAR)
    {
        SPLIT_STR(args_str);
        clean_token(&args_str);
        tok_to_num(step_one, args_str, value);
    }
    else if(IS_EMPTY_STR(args_str))
    {
        return INVALID_MACRO_STATEMENT;
    }
    else /* Whitespace */
    {
        IGNORE_WHITE_SPACES(args_str);
        if(*args_str == MACRO_SET_CHAR)
        {
            SPLIT_STR(args_str);
            clean_token(&args_str);
            tok_to_num(step_one, args_str, value);
        }
        else
        {
            return INVALID_MACRO_STATEMENT;
        }
    }

    return OK;
}

errorCode map_statement(step_one *step_one)
{   
    char *statement_key;
    statement *statement_ref = step_one->curr_statement;
    char *statement_line = step_one->curr_line;

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

    TRY_THROW(map_statement_key(statement_key, statement_ref));
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

errorCode get_next_arg(step_one *step_one, address *address_ref)
{
    char *token;
    address_ref = (address *)malloc(sizeof(address_ref));
    if(address_ref == NULL)
    {
        exit(EXIT_FAILURE);
    }

    TRY_THROW(strtok_wrapper(step_one, &token));
    step_one->curr_statement->args = NULL;
    address_ref->symbol_name = NULL;

    if(IS_NUM_FIRST_CHAR(token))
    {
        TRY_THROW(tok_to_num(step_one, token, &address_ref->value));
        address_ref->type = DATA;
    }
    else if(token[0] == INSTANT_CHAR)
    {
        token++;
        TRY_THROW(tok_to_num(step_one, token, &address_ref->value));
        address_ref->type = INSTANT;
    }
    else if(IS_VALID_REGISTER(token))
    {
        address_ref->value = REGISTER_INDEX(token);
        address_ref->type = REGISTER;
    }
    else if(IS_ARRAY(token))
    {
        TRY_THROW(tok_to_array(step_one, token, address_ref));
    }
    else if(is_valid_tag(token))
    {
        address_ref->symbol_name = token;
        address_ref->type = DIRECT;
    }
    else
    {
        return INVALID_ADDRESS;
    }
    

    return OK;
}

errorCode get_string_arg(step_one *step_one, char **str_ref)
{
    char *args_str = step_one->curr_statement->args;
    clean_token(&args_str);

    /* Check first char after cleaning */
    TRY_THROW((*args_str != QUOTE_CHAR) ? INVALID_STRING : OK);
    args_str++;
    *str_ref = args_str;

    /* Skip charcter until end of string */
    for(;(*args_str != QUOTE_CHAR) && (IS_EMPTY_STR(args_str) == FALSE); args_str++);

    /* Check if the string ends with valid char */
    TRY_THROW((*args_str != QUOTE_CHAR) ? INVALID_STRING : OK); 
    SPLIT_STR(args_str);

    /* Check if there is extraneous charcter after end of string */
    TRY_THROW((IS_EMPTY_STR(args_str) == FALSE) ? INVALID_STRING : OK);

    return OK;
}

/********************/
/***** Privates *****/
/********************/

errorCode map_statement_key(char *statement_key_str, statement *statement_ref)
{
    static translator translator_arr[] = 
    {
        {".data",   DATA_KEY},
        {".string", STRING_KEY},
        {".define", MACRO_KEY},
        {".entry",  ENTRY_KEY},
        {".extern", EXTERN_KEY},
        {NULL, 0}
    };
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
        statement_ref->statement_type = OPERATION_KEY;
        TRY_THROW(map_operation_type(statement_key_str, statement_ref));
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
errorCode strtok_wrapper(step_one *step_one, char **token_ref)
{
    str_len_t i;
    char *args_str = step_one->curr_statement->args;

    TRY_THROW(((*token_ref = strtok(args_str,DELIM)) == NULL) ? MISSING_PARAM : OK);
    TRY_THROW(check_token_consecutive(*token_ref));

    clean_token(token_ref);

    /* Not using strlen because we need only the first 16 chars, effiency */
    for(i = 0; i < MAX_TOKEN_LEN && (*token_ref)[i] != '\0'; i++);
    TRY_THROW((i == MAX_TOKEN_LEN) ? TOK_LEN_EXCEEDED : OK);
    TRY_THROW((IS_EMPTY_STR(*token_ref)) ? (args_str == NULL ? EMPTY_VAL : ARGS_EXPECTED) : OK);
    TRY_THROW(check_cleaned_token(*token_ref));

    return OK;
}

errorCode tok_to_num(step_one *step_one, char *token, word *num_ref)
{
    char *end_str;  /* The pointer to the string after the parsed number */
    symbol *sym;

    /* If the token is macro then take the value from the token, otherwise parse the token string to number */
    sym = find_symbol(step_one->assembler->symbols_table, token);
    if(!(sym && sym->property == MACRO_SYM))
    {
        *num_ref = strtol(token, &end_str, 10);

        /* If there is any character after the read number, that mean that the token didn't containd just a number */
        if(IS_EMPTY_STR(end_str) == FALSE)
        {
            return NOT_WORD_NUM;
        }
    }
    return  OK;
}

errorCode tok_to_array(step_one *step_one, char *token, address *address_ref)
{
    char *index_token; /* The string which represent the index */

    address_ref->type = ARRAY;

    /* Set symbol name of the array */    
    address_ref->symbol_name = token;
    for(; *token != INDEX_START && !IS_EMPTY_STR(token); token++);
    TRY_THROW(IS_EMPTY_STR(token) ? INVALID_ADDRESS : OK);
    SPLIT_STR(token);
    clean_token(&address_ref->symbol_name);
    TRY_THROW(is_valid_tag(address_ref->symbol_name) ? OK : INVALID_TAG);

    /* Skip to first occurence of INDEX_END and validate */
    index_token = token;
    for(; (*token != INDEX_END) && (IS_EMPTY_STR(token) == FALSE); token++);
    TRY_THROW(IS_EMPTY_STR(token) ? INVALID_ADDRESS : OK);
    SPLIT_STR(token);

    /* Validate this is the end of the token */
    TRY_THROW(IS_EMPTY_STR(token) ? OK : INVALID_ADDRESS);

    /* Set number */
    TRY_THROW(tok_to_num(step_one, index_token, &address_ref->value));
    TRY_THROW((address_ref->value < 0) ? INVALID_ADDRESS : OK);
    return OK;
}