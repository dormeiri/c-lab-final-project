#ifndef ASSEMBLER_HEADER
#define ASSEMBLER_HEADER

typedef int word; /* TODO: bitfield */

#include "helpers/queue.h"
#include "helpers/step_one.h"
#include <stdio.h>

#define NUM_OF_REGISTERS 8

/* 2^14 */
#define MAX_NUM 8191
#define MIN_NUM -8191 /* max_num is -8192 no? */

typedef enum
{
    EMPTY,
    COMMENT,
    MACRO,
    STRING_KEY,
    DATA_KEY,
    OPERATION,
    ENTRY,
    EXTERN
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
    char *filename; /* The name of the file without extention (x and not x.ext for example) */
    FILE *outputp;
    FILE *errorsfp;
    queue symbol_table;
    first_step;
} assembler;


/* address, type, symbole, locations[], *next */

typedef struct
{
    queue addresses;
} image_line;

typedef struct
{
    addressingType type;
    word value;
    char *symbol_name;
} address;


errorCode step_one(assembler *assembler);


#endif