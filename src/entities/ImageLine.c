#include <stdio.h>
#include <stdlib.h>
#include "ImageLine.h"

static ErrorCode get_next_address(Queue *addresses, OperationType op_type, OperandDirection op_dir, Address **out);
static Boolean validate_operand(AddressingType ad_type, OperationType op_type, OperandDirection op_dir);

ImageLine *image_line_new()
{
    ImageLine *result;
    if(!(result = (ImageLine *)malloc(sizeof(ImageLine))))
    {
        exit(EXIT_FAILURE);
    }

    result->addresses = queue_new(sizeof(Address));
    return result;
}


/*append operation writes operations and operands to the tmp source code, it verifies the number of operands with 
    the operarion describes.
        Params:
            -step one: pointer to step one struct consistent with the file that's currently handled
            -operation type: enum represents the operation that's handled
            -image line: the image line pointer of the current statement that's habndled
        return: OK if there's a operation-operand match (number wise)
                     or indicates weather there are too many operands or less than requied.*/
ErrorCode image_line_get_operation_word(OperationType op_type, ImageLine *il, Word *out)
{
    char num_of_operands;
    Queue *raw_queue;
    Queue *verified_queue;
    Address *src_address = NULL;
    Address *dest_address = NULL;
    AddressingType src_type = 0;
    AddressingType dest_type = 0;

    num_of_operands = operation_num_of_operands(op_type);
    raw_queue = il->addresses;
    verified_queue = queue_new(sizeof(Address));

    switch (num_of_operands)
    {
        case 2:
            TRY_THROW(get_next_address(raw_queue, op_type, OP_SRC, &src_address));
            src_type = src_address->type;
            src_address->index++;
            enqueue(verified_queue, src_address);
            /* There is no break, it'll continue to case 1 */
        case 1:
            TRY_THROW(get_next_address(raw_queue, op_type, OP_DEST, &dest_address));
            dest_type = dest_address->type;
            if(src_address && src_address->type == REGISTER && dest_address->type == REGISTER)
            {
                src_address->value = dest_address->value | src_address->value;
                address_free(dest_address);
                free(dest_address);
                dest_address = NULL;
            }
            else
            {
                /* If not both of the arguments are registers, enqueue to the verified queue */
                dest_address->index++;
                enqueue(verified_queue, dest_address);
            }
            
            break;
    }
    if(!IS_EMPTY_QUEUE(raw_queue)) 
    {
        return TOO_MANY_OPERANDS;
    }

    queue_free(raw_queue);
    free(raw_queue);
    il->addresses = verified_queue;

    *out = word_operation(
        A_ARE, 
        dest_type, 
        src_type, 
        op_type);

    return OK;
}

void image_line_free(ImageLine *il)
{
    if((il->addresses))
    {
        queue_free(il->addresses);
        free(il->addresses);
        il->addresses = NULL;
    }
}

ErrorCode get_next_address(Queue *addresses, OperationType op_type, OperandDirection op_dir, Address **out)
{
    TRY_THROW((*out = (Address *)dequeue(addresses)) ? OK : MISSING_PARAM);
    TRY_THROW(validate_operand((*out)->type, op_type, op_dir) ? OK : INVALID_ARGUMENT);
    (*out)->value = get_operand_word((*out), op_dir);

    return OK;
}

Boolean validate_operand(AddressingType ad_type, OperationType op_type, OperandDirection op_dir)
{
    switch (op_type)
    {
        case MOV_OP:
        case ADD_OP:
        case SUB_OP:
        case NOT_OP:
        case CLR_OP:
        case DEC_OP:
        case RED_OP:
            return !(op_dir == OP_DEST && ad_type == INSTANT);

        case LEA_OP:
            if(op_dir == OP_DEST)
            {
                return ad_type != INSTANT;
            }
            else
            {
                return ad_type == DIRECT || ad_type == ARRAY;
            }
            
        case JMP_OP:
        case BNE_OP:
        case JSR_OP:
            return ad_type == DIRECT || ad_type == ARRAY;
            
        default:
            return TRUE;
    }
}

