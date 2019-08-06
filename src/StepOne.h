#ifndef STEP_ONE_HEADER
#define STEP_ONE_HEADER

#include "Assembler.h"
#include "helpers/Files.h"
#include "entities/Statement.h"

typedef struct
{
    long address_index;
    int line_counter;
    char *curr_line;            /* line to manipulate during parsing */
    char *curr_line_copy;       /* line to print if any error occured */
    Statement *curr_statement;
    Assembler *assembler;
    Queue *data_image;
} StepOne;

void run_step_one(Assembler *assembler);

#endif