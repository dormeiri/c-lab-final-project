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

/* Comment charecter */
#define COMMENT_CHAR '#'

/* Macro set value charcter */
#define MACRO_SET_CHAR '='

/* Quotes character */
#define QUOTE_CHAR '"'

/* Instant addressing value charecter */
#define INSTANT_CHAR '#'

/* Indicate array index start */
#define INDEX_START '['

/* Indicate array index end */
#define INDEX_END ']'

/* Indicate tag end */
#define TAG_END ':'

/* Indicate register */
#define REGISTER_CHAR 'r'

#define IS_COMMENT(STR) (*(STR) == COMMENT_CHAR)

/* Check if the string represent a number */
#define IS_NUM_FIRST_CHAR(STR) (isdigit((STR)[0]) || (STR)[0] == '-' || (STR)[0] == '+')

/* Check if string is array */
#define IS_ARRAY(STR) (strchr((STR), '['))

/* Split string in its current pointer position */
#define SPLIT_STR(STR) *((STR)++) = '\0'

/* Read statment first word into DEST */
#define READ_FIRST_WORD(STR, DEST)\
    DEST = STR;\
    for(;!(IS_EMPTY_STR(STR) || IS_WHITESPACE(*STR) || *STR == TAG_END); STR++)

/* Read statement second word into DEST */
#define READ_SECOND_WORD(STR, DEST)\
    DEST = STR;\
    for(;(IS_EMPTY_STR(STR) || IS_WHITESPACE(*STR)) == FALSE; STR++)

static ErrorCode map_statement_key(char *statement_key_str, statement *statement_ref);
static ErrorCode map_operation_type(char *statement_key_str, statement *statement_ref);
static ErrorCode tok_to_num(step_one *step_one, char *token, word *out);
static ErrorCode tok_to_array(step_one *step_one, char *token, address *out);
static ErrorCode strtok_wrapper(step_one *step_one, char **tokenp);
static void clean_token(char **token_ref);

/*****************/
/*    Publics    */
/*****************/

/* Check the instruction type of the current line of step_one,
Also check if there is any tag declared
put the result in step one current statement */
ErrorCode map_statement(step_one *step_one)
{   
    char *statement_key;    /* Store the instruction string (example: .data, mov,...) */
    statement *statement = step_one->curr_statement;
    char *statement_line = step_one->curr_line;

    IGNORE_WHITE_SPACES(statement_line);
    if(IS_COMMENT(statement_line)) /* Comment line */
    {
        statement->statement_type = COMMENT_KEY;
        return OK;
    }
    if(IS_EMPTY_STR(statement_line)) /* Empty line */
    {
        statement->statement_type = EMPTY_KEY;
        return OK;
    }
    READ_FIRST_WORD(statement_line, statement->tag);
    if(*statement_line == TAG_END) /* Has tag */
    {
        SPLIT_STR(statement_line);
        IGNORE_WHITE_SPACES(statement_line);
    
        statement_key = statement_line;
        for(;!(IS_EMPTY_STR(statement_line) || IS_WHITESPACE(*statement_line)); statement_line++);
    }
    else
    {
        statement_key = statement->tag;
        statement->tag = NULL;
    }
    
    /* Split instruction string and arguments string */
    if(!(IS_EMPTY_STR(statement_line)))
    {

        SPLIT_STR(statement_line);
        IGNORE_WHITE_SPACES(statement_line);
    }

    statement->args = IS_EMPTY_STR(statement_line) ? NULL : statement_line;

    TRY_THROW(map_statement_key(statement_key, statement));
    TRY_THROW(preaction_validations(statement->args));
    return OK;
}

/* Parse macro statement by splitting the results on MACRO_SET_CHAT */
ErrorCode parse_macro_statement(step_one *step_one, char **symbol, word *value)
{
    char *args_str = step_one->curr_statement->args;
    
    IGNORE_WHITE_SPACES(args_str);
    *symbol = args_str; 

    /* Skip to MACRO_SET_CHAR */
    for(;!(IS_EMPTY_STR(args_str) || *args_str == MACRO_SET_CHAR); args_str++);    

    TRY_THROW(IS_EMPTY_STR(args_str) ? INVALID_MACRO_STATEMENT : OK); /* Check if MACRO_SET_CHAR wasn't found */

    SPLIT_STR(args_str); /* Split on MACRO_SET_CHAR */
    clean_token(symbol);
    clean_token(&args_str);

    TRY_THROW(IS_EMPTY_STR(args_str) ? MISSING_PARAM : OK);
    TRY_THROW(tok_to_num(step_one, args_str, value));

    return OK;
}

/* Get the next argument of step one current statement, put the result in out */
ErrorCode get_next_arg(step_one *step_one, address **out)
{
    char *token;

    if(!(*out = (address *)malloc(sizeof(address))))
    {
        exit(EXIT_FAILURE);
    }

    (*out)->symbol_name = NULL;
    (*out)->value = 0;

    TRY_THROW(strtok_wrapper(step_one, &token));
    step_one->curr_statement->args = NULL;
    (*out)->symbol_name = NULL;

    if(IS_NUM_FIRST_CHAR(token)) /* Data type address */
    {
        TRY_THROW(tok_to_num(step_one, token, &(*out)->value));
        (*out)->type = DATA;
    }
    else if(token[0] == INSTANT_CHAR) /* Instant type address */
    {
        token++;
        TRY_THROW(tok_to_num(step_one, token, &(*out)->value));
        (*out)->type = INSTANT;
    }
    else if(parse_register(token, &(*out)->value)) /* Register type address */
    {
        TRY_THROW(((*out)->value >= NUM_OF_REGISTERS || (*out)->value < 0) ? OUT_OF_RANGE_REGISTER : OK);
        (*out)->type = REGISTER;
    }
    else if(IS_ARRAY(token)) /* Array type address */
    {
        TRY_THROW(tok_to_array(step_one, token, (*out)));
    }
    else if(is_valid_tag(token)) /* Direct type address */
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

/* Clean the arguments string and validate the use of quotes */
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

/* Clean the token end check if it is valid tag */
ErrorCode get_label_arg(step_one *step_one, char **label)
{
    *label = step_one->curr_statement->args;
    clean_token(label);
    return is_valid_tag(*label);
}

/* Find the matching operationType of a string */
operationType parse_operation_type(const char *str)
{
    static Translation translations[] = 
    {
        {"mov", MOV_OP},
        {"cmp", CMP_OP},
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
        
    for(i = 0; translations[i].str && strcmp(translations[i].str, str); i++);
    return translations[i].type;
}

/* Check if the string starts with 'r' if yes parse number comes after, but of the number starts with 0 then it must be 0,
for example r01 is not allowed */
boolean parse_register(const char *token, word *out)
{
    char *end_str;
    if((token[0] != REGISTER_CHAR) || ((token[1] == '0') && (token[2])))
    {
        return FALSE;
    }
    *out = strtol(token + 1, &end_str, 10);

    /* If there is any character after the read number, that mean that the token didn't containd just a number */
    return IS_EMPTY_STR(end_str) ? TRUE : FALSE;
}

/********************/
/*     Privates     */
/********************/

/* Match statement_key_str to instruction type and put the result in statement_ref */
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

/* Match the statement key str to operation string and put in the statement struct the result */
ErrorCode map_operation_type(char *statement_key_str, statement *statement_ref)
{
    operationType op;

    op = parse_operation_type(statement_key_str);
    if(op == NONE_OP)
    {
        return UNDEFINED_COMMAND;
    }
    statement_ref->operation_type = op;

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

/* Parse token to numer,  */
ErrorCode tok_to_num(step_one *step_one, char *token, word *out)
{
    char *end_str;  /* The pointer to the string after the parsed number */
    symbol *sym;

    /* If the token is macro then take the value from the token, otherwise parse the token string to number */
    sym = find_symbol(step_one->assembler->symbols_table, token);

    if((sym && sym->property.prop == MACRO_SYM))
    {
        *out = sym->value;
    }
    else
    {
        *out = strtol(token, &end_str, 10);

        /* If there is any character after the read number, that mean that the token didn't containd just a number */
        if(IS_EMPTY_STR(end_str) == FALSE)
        {
            return NOT_WORD_NUM;
        }
    }
    return  OK;
}

/* Parse token to array and put the result in address_ref */
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