#include <stdlib.h>

#include "Address.h"

Address *address_new()
{
    Address *result;
    if(!(result = (Address *)malloc(sizeof(Address))))
    {
        exit(EXIT_FAILURE);
    }

    result->symbol_name = NULL;
    result->type = 0;
    result->value = 0;
    result->index = 0;

    return result;
}

/*get operand word is a sub routine of append operation to handle operands convertion and classification
    Params:
        -operand: pointer to an Address struct contains an operand
    return: word struct cosistent with the operand.... */
Word get_operand_word(Address *operand, OperandDirection op_dir)
{
    switch (operand->type)
    {
        case REGISTER:
        {
            if(op_dir == OP_SRC)
            {
                return word_register(0, operand->value);
            }
            else
            {
                return word_register(operand->value, 0);
            }
        }
        case INSTANT:
        default:
            return word_push_are(operand->value, A_ARE);
    }
}

char operation_num_of_operands(OperationType op_type)
{
    switch (op_type)
    {
        case MOV_OP: 
        case CMP_OP: 
        case ADD_OP: 
        case SUB_OP: 
        case LEA_OP: 
            return 2;

        case NOT_OP: 
        case CLR_OP: 
        case INC_OP: 
        case DEC_OP: 
        case JMP_OP: 
        case BNE_OP: 
        case RED_OP: 
        case PRN_OP: 
        case JSR_OP: 
            return 1;

        case RTS_OP: 
        case STOP_OP: 
            return 0;

        default:
            return -1;
    }
}

void address_free(Address *a)
{
    if((a->symbol_name))
    {
        free(a->symbol_name);
        a->symbol_name = NULL;
    }

    a->type = 0;
    a->value = 0;
}