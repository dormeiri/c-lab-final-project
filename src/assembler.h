#ifndef ASSEMBLER_HEADER
#define ASSEMBLER_HEADER

#include "helpers/queue.h"
#include <stdio.h>

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
    ARRAY,      
    INDEX,
    REGISTER,
    DATA
} addressingType;

typedef struct
{
    char *name;         /* The name of the assembler program as received from argv (withouth extention) */
    FILE *input_fp;
    FILE *output_fp;    /* Only 1 given file pointer at given time, we don't need to hold ext, ent etc. togeter */
    /* FILE *errors_fp; */    /* Lets talk about it, I came with conclusion that we need to use stderr. */
    boolean succeed;
    queue symbol_table;
    first_step;         /* ? */
} assembler;


/* address, type, symbole, locations[], *next */

typedef struct
{
    queue *addresses;
} image_line;

typedef struct
{
    addressingType type;
    word value;
    char *symbol_name; /* Used for step 1 where value not found in symbol table */
} address;

#endif