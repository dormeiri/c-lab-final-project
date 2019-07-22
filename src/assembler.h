#ifndef ASSEMBLER_HEADER
#define ASSEMBLER_HEADER

#include <stdio.h>
#include "errors.h"
#include "helpers/queue.h"

#define NUM_OF_REGISTERS 8

/* 2^14 */
#define MAX_NUM 8191
#define MIN_NUM -8192

typedef int word; /* TODO: bitfield? */
typedef enum
{
    EMPTY,
    COMMENT,
    MACRO_KEY,
    STRING_KEY,
    DATA_KEY,
    OPERATION_KEY,
    ENTRY_KEY,
    EXTERN_KEY
} statementType;

typedef enum
{
    NONE,
    MOV,
    JMP,
    ADD,
    SUB
    /* TODO: Complete */
} operationType;

typedef enum
{
    INSTANT,    /* Yashir */
    DIRECT,     /* Miadi */
    ARRAY,      /* During step1, the value of address in array type is the value of the index,
                 * During step2, the value of the address is += the value of the symbol */
    REGISTER,
    DATA
} addressingType;

#define SYMBOL_HASHSIZE 100 /* This is the number of available hashes, this is not the limit of the hashset size */

typedef enum 
{
    MACRO_SYM,
    DATA_SYM,
    CODE_SYM,
    EXTERN_SYM,
    ENTRY_SYM
} symbolProperty;

typedef struct 
{
    const char *symbol_name;
    symbolProperty property;
    word value;
    queue *addresses;
} symbol;

typedef struct symbol_list
{
    struct symbol_list *next;
    symbol *value;
} symbol_list;

typedef symbol_list *symbols_table[SYMBOL_HASHSIZE];

typedef struct
{
    const char *name;         /* The name of the assembler program as received from argv (withouth extention) */
    FILE *input_fp;
    FILE *output_fp;    /* Only 1 given file pointer at given time, we don't need to hold ext, ent etc. togeter */
    symbols_table *symbols_table;
    boolean succeed;
} assembler;

#endif