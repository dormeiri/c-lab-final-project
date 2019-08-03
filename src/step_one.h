#ifndef STEP_ONE_HEADER
#define STEP_ONE_HEADER

#include "assembler.h"
#include "helpers/files.h"

typedef struct
{
    addressingType type;
    word value;
    char *symbol_name; /* Used for step 1 where value not found in symbol table */
} address;

typedef struct
{
    Queue *addresses;
} image_line;

typedef struct
{
    char *tag;
    statementType statement_type;
    operationType operation_type;
    char *args;
    image_line *image_line;
} statement;

typedef struct
{
    long address_index;
    int line_counter;
    char *curr_line;            /* line to manipulate during parsing */
    char *curr_line_copy; /* line to print if any error occured */
    statement *curr_statement;
    assembler *assembler;
} step_one;

void run_step_one(assembler *assembler);

#endif