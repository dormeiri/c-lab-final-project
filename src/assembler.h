#ifndef ASSEMBLER_HEADER
#define ASSEMBLER_HEADER

#include <stdio.h>
#include "errors.h"
#include "helpers/list.h"
#include "helpers/queue.h"

#define NUM_OF_REGISTERS 8
#define WORD_SIZE 14
/* 2^WORD_SIZE / 2 - 1 */
#define MAX_NUM 8191
/* 2^WORD_SIZE / 2 */
#define MIN_NUM -8192

typedef unsigned int word;

typedef union
{
    word raw;

    struct
    {
        word are : 2;
        word address_dest : 2;
        word address_src : 2;
        word op_code: 4;
    } operation_word;

    struct
    {
        word are : 2;
        word value : 12;
    } operand_word;
    
} word_converter;

typedef enum
{
    A_ARE = 0,
    R_ARE = 1,
    E_ARE = 2
} are;

typedef enum
{
    EMPTY_KEY,
    COMMENT_KEY,
    MACRO_KEY,
    STRING_KEY,
    DATA_KEY,
    OPERATION_KEY,
    ENTRY_KEY,
    EXTERN_KEY
} statementType;

/* Operations the assmbler recognize, the value of the enums is the exact value of the binary code of the operation */
typedef enum
{
    NONE_OP = -1,
    MOV_OP = 0,
    CMP_OP = 1,
    ADD_OP = 2,
    SUB_OP = 3,
    NOT_OP = 4,
    CLR_OP = 5,
    LEA_OP = 6,
    INC_OP = 7,
    DEC_OP = 8,
    JMP_OP = 9,
    BNE_OP = 10,
    RED_OP = 11,
    PRN_OP = 12,
    JSR_OP = 13,
    RTS_OP = 14,
    STOP_OP = 15
} operationType;

/* The addresses types of the assembler, the values of the enums are the binary value when building operation word */
typedef enum
{
    INSTANT = 0,    /* Yashir */
    DIRECT = 1,     /* Miadi */
    ARRAY = 2,      /* During step1, the value of address in array type is the value of the index,
                    * During step2, the value of the address is += the value of the symbol */
    REGISTER = 3,
    DATA = -1            /* Not in use in operation word so it does not matter what its value */
} addressingType;

/*******************/
/* Symbols defines */
/*******************/

#define SYMBOL_HASHSIZE 100 /* This is the number of available hashes, this is not the limit of the hashset size */

typedef enum 
{
    UNKNOWN_SYM = 0,    /* 0000 in binary */
    MACRO_SYM = 1,      /* 0001 in binary */
    DATA_SYM = 2,       /* 0010 in binary */
    CODE_SYM = 3,       /* 0011 in binary */
    EXTERN_SYM = 4      /* 0100 in binary */
} symbolProperty;

typedef struct
{
    long address_index;
    long file_pos;
    long line_num;
    char *line_str;
} symbol_usage;
typedef struct 
{
    const char *symbol_name;
    struct
    {
        unsigned int prop : 4;
        unsigned int ent : 1;
    } property;
    word value;
    list *usages;
} symbol;

typedef struct symbol_list
{
    struct symbol_list *next;
    symbol *value;
} symbol_list;

typedef symbol_list *symbols_table[SYMBOL_HASHSIZE];

/*************/
/* Assembler */
/*************/

typedef struct
{
    const char *name;               /* The name of the assembler program as received from argv (withouth extention) */
    FILE *input_fp;
    FILE *output_fp;                /* Only 1 given file pointer at given time, we don't need to hold ext, ent etc. togeter */
    symbols_table *symbols_table;
    boolean succeed;
} assembler;


#endif