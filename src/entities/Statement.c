#include <stdlib.h>
#include "Statement.h"

Statement *statement_new()
{
    Statement *result;

    if(!(result = (Statement *)calloc(1, sizeof(Statement))))
    {
        exit(EXIT_FAILURE);
    }
    result->image_line = image_line_new();
    return result;
}

/*get symbol property handles the classification of symols according to its context, its a subroutin of add symbol declaration
    Params:
        -statement: the statement that's its symbol's property we wish to define
    return: the statement type the symbol represents*/
SymbolProperty statement_get_symbol_property(Statement *statement)
{
    switch (statement->statement_type)
    {
        case MACRO_KEY: return MACRO_SYM;
        case OPERATION_KEY: return CODE_SYM;
        default: return DATA_SYM;
    }
}
/*operation operands received an operation names and return a number between 0-2 to represent the number of
operands the operation works on.
    Params: 
        -op_type: the operation that needs to be matched with the number of operands.
    return: 0 if the operation needs no operands in order to operate(e.g. "stop" operation), 
            1 if the operation needs a single operand to operate (e.g. "not" operation) 
            2 if the operation needs two operands to operate (e.g. "mov" operation)  */
char operation_operands(OperationType op_type)
{
    switch (op_type)
    {
        case MOV_OP:
        case CMP_OP:
        case ADD_OP:
        case SUB_OP:
        case LEA_OP: return 2;

        case NOT_OP:
        case CLR_OP:
        case INC_OP:
        case DEC_OP:
        case JMP_OP:
        case BNE_OP:
        case RED_OP:
        case PRN_OP:
        case JSR_OP: return 1;

        case RTS_OP:
        case STOP_OP: return 0;

        default: return -1; /* It should not get here */
    }
}

void statement_free(Statement *s)
{
    if((s->image_line))
    {
        image_line_free(s->image_line);
        free(s->image_line);
        s->image_line = NULL;
    }

    if((s->args))
    {
        free(s->args);
        s->args = NULL;
    }
    if((s->tag))
    {
        free(s->tag);
        s->tag = NULL;
    }
    s->operation_type = 0;
    s->statement_type = 0;
}