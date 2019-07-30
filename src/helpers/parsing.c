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

typedef struct Translation
{
    char *str;
    int type;
    
} Translation;

#define COMMENT_CHAR '#'
#define MACRO_SET_CHAR '='
#define QUOTE_CHAR '"'
#define INSTANT_CHAR '#'
#define INDEX_START '['
#define INDEX_END ']'
#define TAG_END ':'
#define REGISTER_CHAR 'r'
#define REGISTER_INDEX(STR) (STR[1] - '0')
#define IS_COMMENT(STR) (*STR == COMMENT_CHAR)
#define IS_NUM_FIRST_CHAR(STR) (isdigit(STR[0]) || STR[0] == '-' || STR[0] == '+')
#define IS_REGISTER(STR) (STR[0] == REGISTER_CHAR && STR[1] != '\0' && STR[2] == '\0')
#define IS_VALID_REGISTER(STR) (IS_REGISTER(STR) && REGISTER_INDEX(STR) >= 0 && REGISTER_INDEX(STR) < NUM_OF_REGISTERS && STR[2] == '\0')
#define IS_ARRAY(STR) (strchr(STR, '['))

/* Split string in its current pointer position */
#define SPLIT_STR(STR) *(STR++) = '\0'

/* Read statment first word into DEST */
#define READ_FIRST_WORD(STR, DEST) \
    DEST = STR;\
    for(;(IS_EMPTY_STR(STR) || IS_WHITESPACE(*STR) || *STR == TAG_END) == FALSE; STR++)

/* Read statement second word into DEST */
#define READ_SECOND_WORD(STR, DEST) \
    DEST = STR;\
    for(;(IS_EMPTY_STR(STR) || IS_WHITESPACE(*STR)) == FALSE; STR++)

static ErrorCode tok_to_num(step_one *step_one, char *token, word *num_ref);
static ErrorCode tok_to_array(step_one *step_one, char *token, address *out);
static ErrorCode strtok_wrapper(step_one *step_one, char **tokenp);
static ErrorCode map_statement_key(char *statement_key_str, statement *statement_ref);
static ErrorCode map_operation_type(char *statement_key_str, statement *statement_ref);

ErrorCode get_label_arg(step_one *step_one, char **label)
{
    *label = step_one->curr_statement->args;
    clean_token(label);
    return is_valid_tag(*label);
}

ErrorCode parse_macro_statement(step_one *step_one, char **symbol, word *value)
{
    /* TODO: Break down to smaller funcitons, try to remove duplicates */

    char *args_str;

    args_str = step_one->curr_statement->args;
    IGNORE_WHITE_SPACES(args_str);
    *symbol = args_str;
    for(;!(IS_EMPTY_STR(args_str) || IS_WHITESPACE(*args_str) || *args_str == MACRO_SET_CHAR); args_str++);
    IGNORE_WHITE_SPACES(args_str);
    if(*args_str != MACRO_SET_CHAR)
    {
        return INVALID_MACRO_STATEMENT;
    }

    SPLIT_STR(args_str);
    clean_token(symbol);
    clean_token(&args_str);

    TRY_THROW(IS_EMPTY_STR(args_str) ? MISSING_PARAM : OK);

    TRY_THROW(tok_to_num(step_one, args_str, value));

    return OK;
}

ErrorCode map_statement(step_one *step_one)
{   
    char *statement_key;
    statement *statement = step_one->curr_statement;
    char *statement_line = step_one->curr_line;

    IGNORE_WHITE_SPACES(statement_line);
    if(IS_COMMENT(statement_line))
    {
        statement->statement_type = COMMENT_KEY;
        return OK;
    }
    if(IS_EMPTY_STR(statement_line))
    {
        statement->statement_type = EMPTY_KEY;
        return OK;
    }

    READ_FIRST_WORD(statement_line, statement->tag);
    if(*statement_line == TAG_END)
    {
        SPLIT_STR(statement_line);
        IGNORE_WHITE_SPACES(statement_line);
        READ_SECOND_WORD(statement_line, statement_key);
    }
    else
    {
        statement_key = statement->tag;
        statement->tag = NULL;
    }
    
    if(IS_EMPTY_STR(statement_line) == FALSE)
    {
        SPLIT_STR(statement_line);
        IGNORE_WHITE_SPACES(statement_line);
    }
    statement->args = IS_EMPTY_STR(statement_line) ? NULL : statement_line;

    TRY_THROW(map_statement_key(statement_key, statement));
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

ErrorCode get_next_arg(step_one *step_one, address **out)
{
    char *token;
    *out = (address *)malloc(sizeof(out));
    (*out)->symbol_name = NULL;
    (*out)->value = 0;

    if(out == NULL)
    {
        exit(EXIT_FAILURE);
    }

    TRY_THROW(strtok_wrapper(step_one, &token));
    step_one->curr_statement->args = NULL;
    (*out)->symbol_name = NULL;

    if(IS_NUM_FIRST_CHAR(token))
    {
        TRY_THROW(tok_to_num(step_one, token, &(*out)->value));
        (*out)->type = DATA;
    }
    else if(token[0] == INSTANT_CHAR)
    {
        token++;
        TRY_THROW(tok_to_num(step_one, token, &(*out)->value));
        (*out)->type = INSTANT;
    }
    else if(IS_VALID_REGISTER(token))
    {
        (*out)->value = REGISTER_INDEX(token);
        (*out)->type = REGISTER;
    }
    else if(IS_ARRAY(token))
    {
        TRY_THROW(tok_to_array(step_one, token, (*out)));
    }
    else if(is_valid_tag(token))
    {
        symbol *sym;
        if((sym = find_symbol(step_one->assembler->symbols_table, token)) && sym->property.prop == MACRO_SYM)
        {
            (*out)->value = sym->value;
            (*out)->type = DATA;
        }
        else
        {
            (*out)->symbol_name = token;
            (*out)->type = DIRECT;
        }
    }
    else
    {
        return INVALID_ADDRESS;
    }
    

    return OK;
}

ErrorCode get_string_arg(step_one *step_one, char **str_ref)
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

ErrorCode map_statement_key(char *statement_key_str, statement *statement_ref)
{
    static Translation translations[] = 
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
        for(i = 0; (translations[i].str) && strcmp(translations[i].str, statement_key_str); i++);
        if(!(translations[i].str))
        {
            return UNDEFINED_COMMAND; /* TODO: Undefined statement key? undefined operation? */
        }
        statement_ref->statement_type = translations[i].type;
        statement_ref->operation_type = NONE_OP;
    }
    else
    {
        statement_ref->statement_type = OPERATION_KEY;
        TRY_THROW(map_operation_type(statement_key_str, statement_ref));
    }

    return OK;
}

ErrorCode map_operation_type(char *statement_key_str, statement *statement_ref)
{
    static Translation translations[] = 
    {
        {"mov", MOV_OP},
        {"add", ADD_OP},
        {"sub", SUB_OP},
        {"not", NOT_OP},
        {"clr", CLR_OP},
        {"lea", LEA_OP},
        {"inc", INC_OP},
        {"dec", DEC_OP},
        {"jmp", JMP_OP},
        {"bne", BNE_OP},
        {"red", RED_OP},
        {"prn", PRN_OP},
        {"jsr", JSR_OP},
        {"rts", RTS_OP},
        {"stop", STOP_OP},
        {NULL, NONE_OP}
    };
    unsigned char i;

    for(i = 0; strcmp(translations[i].str, statement_key_str); i++);
    if(translations[i].str == NULL)
    {
        return UNDEFINED_COMMAND; /* TODO: Undefined statement key? undefined operation? */
    }
    statement_ref->operation_type = translations[i].type;

    return OK;
}

/* Parse the next token with strtok, clean this token from leading and trailing white spaces,
and check if the token is valid pass args_str as NULL to continue reading tokens from the last string */
ErrorCode strtok_wrapper(step_one *step_one, char **token_ref)
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

ErrorCode tok_to_num(step_one *step_one, char *token, word *num_ref)
{
    char *end_str;  /* The pointer to the string after the parsed number */
    symbol *sym;

    /* If the token is macro then take the value from the token, otherwise parse the token string to number */
    sym = find_symbol(step_one->assembler->symbols_table, token);

    if((sym && sym->property.prop == MACRO_SYM))
    {
        *num_ref = sym->value;
    }
    else
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

ErrorCode tok_to_array(step_one *step_one, char *token, address *address_ref)
{
    char *index_token; /* The string which represent the index */

    address_ref->type = ARRAY;
    
    address_ref->symbol_name = token;
    for(; *token != INDEX_START && !IS_EMPTY_STR(token); token++);
    TRY_THROW(IS_EMPTY_STR(token) ? MISSING_ARRAY_BRACE : OK);
    SPLIT_STR(token);
    clean_token(&address_ref->symbol_name);
    TRY_THROW(is_valid_tag(address_ref->symbol_name) ? OK : INVALID_TAG);

    /* Skip to first occurence of INDEX_END and validate */
    index_token = token;
    for(; (*token != INDEX_END) && !IS_EMPTY_STR(token); token++);
    TRY_THROW(IS_EMPTY_STR(token) ? MISSING_ARRAY_BRACE : OK);
    SPLIT_STR(token);

    /* Validate this is the end of the token */
    TRY_THROW(IS_EMPTY_STR(token) ? OK : AFTER_TEXT);

    /* Set number */
    TRY_THROW(tok_to_num(step_one, index_token, &address_ref->value));
    TRY_THROW((address_ref->value < 0) ? NOT_ARRAY_INDEX : OK);

    return OK;
}